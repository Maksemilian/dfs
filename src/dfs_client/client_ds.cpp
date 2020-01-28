#include "client_ds.h"

#include "client_signal_stream_server.h"

#include "receiver.pb.h"

#include <QByteArray>
#include <QDataStream>
#include <QTimer>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QHostAddress>

//**************************************** Receiver Station Client*****************************************

struct DeviceClient::Impl
{
    Impl()
    {
    }
    QQueue<proto::receiver::CommandType>commandQueue;
    proto::receiver::DeviceSetInfo currentDeviceSetInfo;
    SignalStreamServer streamServer;
};

DeviceClient::DeviceClient(const ConnectData& connectData, QObject* parent)
    :    Client (connectData, parent),
         d(std::make_unique<Impl>())
{
}

void DeviceClient::setLiceningStreamPort(quint16 port)
{
    d->streamServer.listen(QHostAddress::Any, port);
}

quint16 DeviceClient::liceningStreamPort()
{
    return d->streamServer.serverPort();
}

const proto::receiver::DeviceSetInfo& DeviceClient::getDeviceSetInfo() const
{
    return d->currentDeviceSetInfo;
}

QStringList DeviceClient::receiverNameList()const
{
    QStringList receiverNameList;

    const proto::receiver::DeviceSetInfo& deviceSetInfo = getDeviceSetInfo();
    for(int i = 0; i < deviceSetInfo.device_info_size(); i++)
    {
        receiverNameList << deviceSetInfo.device_info(i).serial_number().data();
    }
    return receiverNameList;
}

ShPtrPacketBuffer DeviceClient::getDDC1Buffer() const
{
    return  d->streamServer.getBuffer(SignalStreamServer::StreamType::ST_DDC1);
}

DeviceClient::~DeviceClient() {}

QString DeviceClient::getCurrentDeviceSetName()const
{
    QString deviceSetName = "DS#";
    for (int i = 0; i < d->currentDeviceSetInfo.device_info_size(); i++)
    {
        deviceSetName += d->currentDeviceSetInfo.device_info(i).serial_number().data();
        qDebug() << QString(d->currentDeviceSetInfo.device_info(i).serial_number().data());
        if(i != d->currentDeviceSetInfo.device_info_size() - 1)
        {
            deviceSetName += "_";
        }
    }
    return deviceSetName;
}

void DeviceClient::onMessageReceived(const QByteArray& buffer)
{
    //qDebug()<<"ReceiverStationClient::onMessageReceived"<<buffer.size();
    proto::receiver::HostToClient hostToClient;
    if(!hostToClient.ParseFromArray(buffer.constData(), buffer.size()))
    {
        qDebug() << "ERROR PARSE HOST_TO_CLIENT_MESSAGE";
        return;
    }

    if(hostToClient.has_command_answer())
    {
        proto::receiver::Answer commandAnswer = hostToClient.command_answer();
        readAnswerPacket(commandAnswer);
    }
    else if (hostToClient.has_device_set_info())
    {
        d->currentDeviceSetInfo = hostToClient.device_set_info();
        //        qDebug()<<"DeviceSetReadyForUse";
//        emit deviceInfoUpdated(receiverNameList());
        emit deviceReady(receiverNameList());
    }
    else if (hostToClient.is_ready())
    {
        emit deviceReady(receiverNameList());
    }
    else
    {
        qDebug() << "ERROR MESSAGE RECEIVE";
    }
}

void DeviceClient::readAnswerPacket(const proto::receiver::Answer& answer)
{
    if(d->commandQueue.isEmpty())
    {
        qDebug() << "ERROR QUEUE IS EMPTY";
        return;
    }

    if(answer.type() != d->commandQueue.head())
    {
        qDebug() << "ERROR Answer type";
        return;
    }

    if(answer.succesed())
    {
        switch (answer.type())
        {
            case proto::receiver::CommandType::SET_POWER_OFF:
                qDebug() << "SETED_POWER_OFF";
                //            emit deviceSetPowerSetted(false);
                break;
            case proto::receiver::CommandType::SET_POWER_ON:
                qDebug() << "SETED_POWER_ON";
                //            emit deviceSetPowerSetted(true);
                break;
            case proto::receiver::CommandType::SET_SETTINGS:
                qDebug() << "SETED_SETTINGS:";
                //            emit deviceSetSettingsSetted();
                break;
            case proto::receiver::CommandType::SET_ATTENUATOR:
                qDebug() << "SETED_ATTENUATOR" << objectName();
                break;
            case proto::receiver::CommandType::SET_PREAMPLIFIER_ENABLED:
                qDebug() << "SETED_PREAMPLIFIER_ENABLED";
                break;
            case proto::receiver::CommandType::SET_PRESELECTORS:
                qDebug() << "SETED_PRESELECTORS";
                break;
            case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
                qDebug() << "SETED_NOICE_BLANKER_ENABLED";
                break;
            case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
                qDebug() << "SETED_ADC_NOICE_BLANKER_THRESHOLD";
                break;
            case proto::receiver::CommandType::SET_DDC1_FREQUENCY:
                qDebug() << "SETED_FREQUENCY";
                break;
            case proto::receiver::CommandType::SET_DDC1_TYPE:
                qDebug() << "SETED_DDC1_TYPE";
                //            emit deviceSetChangeBandwith();
                break;
            case proto::receiver::CommandType::START_DDC1:
                qDebug() << "STARTED_DDC1:";
                emit ddc1StreamStarted();
                break;
            case proto::receiver::CommandType::STOP_DDC1:
                qDebug() << "STOPED_DDC1:";
                emit ddc1StreamStoped();
                break;
            case proto::receiver::CommandType::SET_SHIFT_PHASE_DDC:
                qDebug() << "SETTED_SHIFT_PHASE_DDC1";
                break;
            case proto::receiver::SET_DEVICE_INDEX:
                qDebug() << "SETTED_DEVICE_SET_INDEX";
                break;
            case proto::receiver::START_SENDING_DDC1_STREAM:
                qDebug() << "START_SENDING_DDC1_STREAM";
                break;
            case proto::receiver::STOP_SENDING_DDC1_STREAM:
                qDebug() << "STOP_SENDING_DDC1_STREAM";
                break;
            case proto::receiver::SET_DEVICE_MODE:
            case proto::receiver::UNKNOWN_COMMAND:
            case proto::receiver::CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
            case proto::receiver::CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
                qDebug() << "WARNING:" << "UNUSED ENUMERATION VALUE";
                break;
        }
        emit commandSuccessed();
    }
    else
    {
        qWarning() << "ERROR RESPONSE" << answer.type();
        emit commandFailed(errorString(answer.type()));
    }
    //    qDebug()<<"DEQ_B";
    d->commandQueue.dequeue();
    //    qDebug()<<"DEQ_E";
}

void DeviceClient::sendCommand(proto::receiver::Command& command)
{
    if(command.command_type() == proto::receiver::START_SENDING_DDC1_STREAM)
    {
        command.set_stream_port(d->streamServer.serverPort());
    }
    d->commandQueue.enqueue(command.command_type());
    proto::receiver::ClientToHost clientToHost;
    //TODO ПОНЯТЬ КАК РАБОТАЕТ
    clientToHost.mutable_command()->CopyFrom(command);
    // d->channel->writeToConnection(serializeMessage(clientToHost));
    sendMessage(clientToHost);
}

QString DeviceClient::errorString(proto::receiver::CommandType commandType)
{
    switch (commandType)
    {
        case proto::receiver::CommandType::SET_POWER_ON:
            return "ERROR Command Power on";
        case proto::receiver::CommandType::SET_POWER_OFF:
            return "ERROR Command Power off ";
        case proto::receiver::CommandType::SET_SETTINGS:
            return "ERROR Command Power set settings";
        case proto::receiver::START_DDC1:
            return"ERROR Command Power start ddc1";
        case proto::receiver::STOP_DDC1:
            return"ERROR Command Power stop ddc1";
        case proto::receiver::SET_DDC1_TYPE:
            return "ERROR Command Power set ddc1 type";
        case proto::receiver::SET_DDC1_FREQUENCY:
            return "ERROR Change Freq";
        case proto::receiver::SET_PRESELECTORS:
            return "ERROR Presselector check";
        case proto::receiver::SET_PREAMPLIFIER_ENABLED:
            return "ERROR Pream check";
        case proto::receiver::SET_ADC_NOICE_BLANKER_ENABLED:
            return"ERROR ADC ENABLED CHECK";
        case proto::receiver::SET_ADC_NOICE_BLANKER_THRESHOLD:
            return"ERROR ADC THRESHOLD CHECK";
        case proto::receiver::SET_ATTENUATOR:
            return "ERROR ATTENUATOR CHECK";
        case proto::receiver::SET_SHIFT_PHASE_DDC:
            return "ERROR SHIFT PHASE DDC1";
        case proto::receiver::SET_DEVICE_INDEX:
            return "ERROR COMMAND SET DEVICE SET INDEX";
        default:
            return "UNKNOWN KOMMAND";
    }
}

/*

void DeviceSetClient::setSettings(const DeviceSetSettings &settings)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::CommandType::SET_SETTINGS);

    command.set_attenuator(settings.attenuator);
    proto::receiver::Preselectors *preselectors=new proto::receiver::Preselectors;
    preselectors->set_low_frequency(settings.preselectors.first);
    preselectors->set_high_frequency(settings.preselectors.second);
    command.set_allocated_preselectors(preselectors);

    command.set_preamplifier_enebled(settings.preamplifier);
    command.set_adc_noice_blanker_enebled(settings.adcEnabled);

    quint16 threshold=settings.threshold;
    void *value=&threshold;
    command.set_adc_noice_blanker_threshold(value,sizeof(threshold));

    command.set_ddc1_type(settings.ddcType);
    command.set_samples_per_buffer(settings.samplesPerBuffer);
    command.set_ddc1_frequency(settings.frequency);

//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setPower(bool state)
{
    proto::receiver::Command command;
    state ? command.set_command_type(proto::receiver::SET_POWER_ON):
            command.set_command_type(proto::receiver::SET_POWER_OFF);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setAttenuator(quint32 attenuator)
{
    qDebug()<<"Set Attenuator";
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ATTENUATOR);
    command.set_attenuator(attenuator);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::startDDC1Stream(quint32 samplesPerBuffer)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::START_DDC1);
    command.set_samples_per_buffer(samplesPerBuffer);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::stopDDC1Stream()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::STOP_DDC1);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setPreselectors(quint32 lowFrequency,quint32 highFrequency)
{
    qDebug()<<"Set Pres";
    proto::receiver::Command command;
    proto::receiver::Preselectors *preselectors=new proto::receiver::Preselectors;
    preselectors->set_low_frequency(lowFrequency);
    preselectors->set_high_frequency(highFrequency);
    command.set_command_type(proto::receiver::SET_PRESELECTORS);
    command.set_allocated_preselectors(preselectors);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setAdcNoiceBlankerEnabled(bool state)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_ENABLED);
    command.set_adc_noice_blanker_enebled(state);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setAdcNoiceBlankerThreshold(quint16 threshold)
{
    proto::receiver::Command command;
    void *value=&threshold;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_THRESHOLD);
    command.set_adc_noice_blanker_threshold(value,sizeof(threshold));
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setPreamplifierEnabled(bool state)
{
    qDebug()<<"Set Pream";
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_PREAMPLIFIER_ENABLED);
    command.set_preamplifier_enebled(state);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setDDC1Frequency(quint32 ddc1Frequency)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_FREQUENCY);
    command.set_ddc1_frequency(ddc1Frequency);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}

void DeviceSetClient::setDDC1Type(quint32 typeIndex)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_TYPE);
    command.set_ddc1_type(typeIndex);
//    d->commandQueue.enqueue(command.command_type());
    sendCommand(command);
}
*/

//************************** DEL *************************

//void ReceiverStationClient::setCurrentCommandType(quint32 type)
//{
//    for(auto it=d->map.begin();it!=d->map.end();it++)
//        it.value()=false;

//    d->map[type]=true;
//}
//bool ReceiverStationClient::checkStateCommand(quint32 type)
//{
//    qDebug()<<"EQU CHECK COMMAND"<<type<<d->map[type];
//    if(d->map.contains(type)){
//        return d->map[type];
//    }
//    return false;
//}

//void ReceiverStationClient::startLoadingFiles(const QStringList &fileNames)
//{
//    if(!d->fileLoaders.isEmpty())return;

//    qDebug()<<"START LOADING FILES";
//    d->fileLoaders<<new FileLoader(d->channel->peerAddress(),8000,fileNames);
//    connect(d->fileLoaders.first(),&FileLoader::fullSizeFile,
//            this,&ReceiverStationClient::receivedFileSize);

//    connect(d->fileLoaders.first(),&FileLoader::bytesProgressFile,
//            this,&ReceiverStationClient::bytesProgressFile);

//    connect(d->fileLoaders.first(),&FileLoader::finished,
//            this,&ReceiverStationClient::stopLoadingFiles);

//    d->fileLoaders.first()->start();
//}

//void ReceiverStationClient::stopLoadingFiles()
//{
//    d->fileLoaders.first()->stop();
//    d->fileLoaders.removeFirst();
//    qDebug()<<"STOP LOADING FILES";
//}
/*
void ReceiverStationClient::onReadyReadInfo()
{
    bool isTakeAnswer=false;
    //static qint64 answerSize=0;
    if(d->answerSize==0){
        if(d->channel->socketBytesAvailable()>=sizeof(int)){
            d->channel->readFromSocket(sizeof(int));
            char buf[sizeof(int)];
            d->channel->readDataFromBuffer(buf,sizeof(int));
            bool ok;
            d->answerSize= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
            //qDebug()<<"Size"<<size;
        }
    }else{
        if(d->channel->socketBytesAvailable()>0){
            qint64 bytes=d->channel->readFromSocket(d->answerSize);
            if(bytes==d->answerSize){
                qDebug()<<"onReadyReadInfo"<<d->answerSize;
                std::vector<char>data(static_cast<unsigned int>(d->answerSize));
                //                char data[d->answerSize];
                d->channel->readDataFromBuffer(data.data(),d->answerSize);
                d->info.ParseFromArray(data.data(),static_cast<int>(d->answerSize));

                d->answerSize=0;
                isTakeAnswer=true;

                disconnect(d->channel.get(),&PeerWireClient::readyRead,this,&ReceiverStationClient::onReadyReadInfo);

                emit deviceSetReadyForUse();
                //emit stationDataReady(socket->peerAddress(),socket->peerPort(),info);
                //qDebug()<<"Answer"<<answer.type();
            }
        }
    }
    if(!isTakeAnswer)onReadyReadInfo();
}
*/
/*
 *
void ReceiverStationClient::transfer()
{
    bool isTakeAnswer=false;
    //static qint64 answerSize=0;
    if(d->answerSize==0&&
            d->channel->socketBytesAvailable()>=sizeof(int)){

        d->channel->readFromSocket(sizeof(int));
        char buf[sizeof(int)];
        d->channel->readDataFromBuffer(buf,sizeof(int));
        bool ok;
        d->answerSize= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        //qDebug()<<"Size"<<size;
    }else if(d->channel->socketBytesAvailable()>0){
        qint64 bytes=d->channel->readFromSocket(d->answerSize);
        if(bytes==d->answerSize){
            //qDebug("Rec");
            try{
                qDebug()<<"AS:"<<d->answerSize;
                Answer answer;
                std::vector<char>data(static_cast<unsigned int>(d->answerSize));
                d->channel->readDataFromBuffer(data.data(),d->answerSize);
                answer.ParseFromArray(data.data(),static_cast<int>(d->answerSize));
                d->answerSize=0;
                isTakeAnswer=true;
                //qDebug()<<"Answer"<<answer.type();
                readAnswerPacket(answer);
            } catch (const std::bad_alloc& err) {
                qDebug()<<"EXCEPTION"<<err.what()<<"AS:"<<d->answerSize;
            }
        }
    }

    if(!isTakeAnswer) sheduleTransfer();
}


void ReceiverStationClient::onReadyReadAnswer()
{
    bool isTakeAnswer=false;
    //static qint64 answerSize=0;
    if(d->answerSize==0&&
            d->channel->socketBytesAvailable()>=sizeof(int)){

        d->channel->readFromSocket(sizeof(int));
        char buf[sizeof(int)];
        d->channel->readDataFromBuffer(buf,sizeof(int));
        bool ok;
        d->answerSize= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        //qDebug()<<"Size"<<size;
    }else if(d->channel->socketBytesAvailable()>0){
        qint64 bytes=d->channel->readFromSocket(d->answerSize);
        if(bytes==d->answerSize){
            //qDebug("Rec");
            qDebug()<<"onReadyReadAnswer"<<d->answerSize;
            Answer answer;
            std::vector<char>data(static_cast<unsigned int>(d->answerSize));
            d->channel->readDataFromBuffer(data.data(),d->answerSize);
            answer.ParseFromArray(data.data(),static_cast<int>(d->answerSize));
            d->answerSize=0;
            isTakeAnswer=true;
            //qDebug()<<"Answer"<<answer.type();
        }
    }

    if(!isTakeAnswer)onReadyReadAnswer();
}

*/

