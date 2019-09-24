#include "receiver_station_client.h"

#include "interface/i_device_set_settings.h"

#include "peer_wire_client.h"
#include "channel.h"
#include "channel_host.h"

#include "packet.pb.h"
#include "device_set_info.pb.h"
//#include "command.pb.h"
#include "receiver.pb.h"

#include "ring_packet_buffer.h"
#include "file_loader.h"

#include <string>

#include <QByteArray>
#include <QDataStream>
#include <QTimer>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QHostAddress>


//**************************************** Receiver Station Client*****************************************

const QByteArray ReceiverStationClient::serializeCommandToByteArray(
        const google::protobuf::Message &command)
{
    //        qDebug()<<"Serialize B"<<command.ByteSize();
    std::vector<char>bytesArray(static_cast<unsigned int>(command.ByteSize()));
    command.SerializeToArray(static_cast<void*>(bytesArray.data()),command.ByteSize());
    //        qDebug()<<"Serialize E";
    return QByteArray(bytesArray.data(),command.ByteSize());
}

struct ReceiverStationClient::Impl
{
    Impl(qintptr socketDescriptor):channel(std::make_unique<ChannelHost>(socketDescriptor))
    {
    }
    std::unique_ptr<StreamReader>streamReader;
    std::unique_ptr<ChannelHost> channel;
    proto::receiver::DeviceSetInfo info;
    qint64 answerSize=0;
    QMap<quint32,bool>map;
    QList<FileLoader*>fileLoaders;
};

ReceiverStationClient::ReceiverStationClient(qintptr socketDescriptor, QObject *parent):
    QObject(parent),
    d(std::make_unique<Impl>(socketDescriptor))
{
    connect(d->channel.get(),&ChannelHost::messageReceived,
            this,&ReceiverStationClient::onMessageReceived);

    connect(d->channel.get(),&ChannelHost::finished,
            this,&ReceiverStationClient::disconnected);

    connect(this,&ReceiverStationClient::deviceSetReadyForUse,
            this,&ReceiverStationClient::connected);

    connect(this,&ReceiverStationClient::deviceSetChangeBandwith,[this]{
        d->streamReader->resetBuffer();
    });

    connect(this,&ReceiverStationClient::ddc1StreamStarted,
            this,&ReceiverStationClient::startDDC1StreamReader);

    connect(this,&ReceiverStationClient::ddc1StreamStoped,
            this,&ReceiverStationClient::stopDDC1StreamReader);
}

ReceiverStationClient::~ReceiverStationClient()
{
    qDebug("DESTRUCTOR RECEIVER STATION CLIENT");
    stopDDC1StreamReader();
    qDebug("STOP STREAM DESTR");
}

QString ReceiverStationClient::getCurrentDeviceSetName()
{
    QString deviceSetName="DS#";
    for (int i=0;i<d->info.device_info_size();i++){
        deviceSetName+=d->info.device_info(i).serial_number().data();
        qDebug()<<QString(d->info.device_info(i).serial_number().data());
        if(i!=d->info.device_info_size()-1){
            deviceSetName+="_";
        }
    }
    return deviceSetName;
}

void ReceiverStationClient::onMessageReceived(const QByteArray &buffer)
{
    qDebug()<<"ReceiverStationClient::onMessageReceived";
    proto::receiver::HostToClient hostToClient;
    if(!hostToClient.ParseFromArray(buffer.constData(),buffer.size())){
        qDebug()<<"ERROR PARSE HOST_TO_CLIENT_MESSAGE";
        return;
    }

    if(hostToClient.has_command_answer()){
      proto::receiver::Answer commandAnswer= hostToClient.command_answer();
      readAnswerPacket(commandAnswer);
    }else if (hostToClient.has_device_set_info()) {
      d->info=hostToClient.device_set_info();
      emit deviceSetReadyForUse();
    }else qDebug()<<"ERROR MESSAGE RECEIVE";
}

void ReceiverStationClient::setCurrentCommandType(quint32 type)
{
    for(auto it=d->map.begin();it!=d->map.end();it++)
        it.value()=false;

    d->map[type]=true;
}

void ReceiverStationClient::readAnswerPacket(const proto::receiver::Answer &answer)
{
    if(answer.succesed()){
        //qDebug()<<"ANSWER"<<answer.type()<<answer.succesed();
        switch (answer.type()) {
        case proto::receiver::CommandType::SET_POWER_OFF:
            qDebug()<<"SETED_POWER_OFF";
            setCurrentCommandType(proto::receiver::CommandType::SET_POWER_OFF);
            emit deviceSetPowerSetted(false);
            break;
        case proto::receiver::CommandType::SET_POWER_ON:
            qDebug()<<"SETED_POWER_ON";
            setCurrentCommandType(proto::receiver::CommandType::SET_POWER_ON);
            emit deviceSetPowerSetted(true);
            break;
        case proto::receiver::CommandType::SET_SETTINGS:
//            qDebug()<<"SETED_SETTINGS:"<<d->channel->peerAddress()<<d->channel->peerPort();
            setCurrentCommandType(proto::receiver::CommandType::SET_SETTINGS);
            emit deviceSetSettingsSetted();
            break;
        case proto::receiver::CommandType::SET_ATTENUATOR:
            qDebug()<< "SETED_ATTENUATOR";
            setCurrentCommandType(proto::receiver::CommandType::SET_ATTENUATOR);
            break;
        case proto::receiver::CommandType::SET_PREAMPLIFIER_ENABLED:
            qDebug()<< "SETED_PREAMPLIFIER_ENABLED";
            setCurrentCommandType(proto::receiver::CommandType::SET_PREAMPLIFIER_ENABLED);
            break;
        case proto::receiver::CommandType::SET_PRESELECTORS:
            qDebug()<< "SETED_PRESELECTORS";
            setCurrentCommandType(proto::receiver::CommandType::SET_PRESELECTORS);
            break;
        case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
            qDebug()<< "SETED_NOICE_BLANKER_ENABLED";
            setCurrentCommandType(proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_ENABLED);
            break;
        case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
            qDebug()<< "SETED_ADC_NOICE_BLANKER_THRESHOLD";
            setCurrentCommandType(proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD);
            break;
        case proto::receiver::CommandType::SET_DDC1_FREQUENCY:
            qDebug()<< "SETED_FREQUENCY";
            setCurrentCommandType(proto::receiver::CommandType::SET_DDC1_FREQUENCY);
            break;
        case proto::receiver::CommandType::SET_DDC1_TYPE:
            setCurrentCommandType(proto::receiver::CommandType::SET_DDC1_TYPE);
            emit deviceSetChangeBandwith();
            qDebug()<< "SETED_DDC1_TYPE";
            break;
        case proto::receiver::CommandType::START_DDC1:
//            qDebug()<<"STARTED_DDC1:"<<d->channel->peerAddress()<<d->channel->peerPort();
            setCurrentCommandType(proto::receiver::CommandType::START_DDC1);
            emit ddc1StreamStarted();
            break;
        case proto::receiver::CommandType::STOP_DDC1:
//            qDebug()<<"STOPED_DDC1:"<<d->channel->peerAddress()<<d->channel->peerPort();
            setCurrentCommandType(proto::receiver::CommandType::STOP_DDC1);
            emit ddc1StreamStoped();
            break;
        case proto::receiver::CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
        case proto::receiver::CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
            break;
        }
    }else{
        qWarning()<<"ERROR RESPONSE"<<answer.type();
    }
}

bool ReceiverStationClient::checkStateCommand(quint32 type)
{
    qDebug()<<"EQU CHECK COMMAND"<<type<<d->map[type];
    if(d->map.contains(type)){
        return d->map[type];
    }
    return false;
}

void ReceiverStationClient::sendCommand(proto::receiver::Command &command)
{
       proto::receiver::ClientToHost clientToHost;
       clientToHost.set_allocated_command(&command);
       d->channel->writeToConnection(serializeCommandToByteArray(clientToHost));
}

//***************************** COMMAND SENDING ***************************

void ReceiverStationClient::setSettings(const DeviceSetSettings &settings)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::CommandType::SET_SETTINGS);

    command.set_attenuator(settings.attenuator);
    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
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

    sendCommand(command);
}

void ReceiverStationClient::setPower(bool state)
{
    proto::receiver::Command command;
    state ? command.set_command_type(proto::receiver::SET_POWER_ON):
            command.set_command_type(proto::receiver::SET_POWER_OFF);
    sendCommand(command);
}

void ReceiverStationClient::setAttenuator(quint32 attenuator)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ATTENUATOR);
    command.set_attenuator(attenuator);
    sendCommand(command);
}

void ReceiverStationClient::startDDC1Stream(quint32 samplesPerBuffer)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::START_DDC1);
    command.set_samples_per_buffer(samplesPerBuffer);
    sendCommand(command);
}

void ReceiverStationClient::stopDDC1Stream()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::STOP_DDC1);
    sendCommand(command);
}

void ReceiverStationClient::setPreselectors(quint32 lowFrequency,quint32 highFrequency)
{
    proto::receiver::Command command;
    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
    proto::receiver::Preselectors *preselectors=new proto::receiver::Preselectors;
    preselectors->set_low_frequency(lowFrequency);
    preselectors->set_high_frequency(highFrequency);
    command.set_command_type(proto::receiver::SET_PRESELECTORS);
    command.set_allocated_preselectors(preselectors);
    sendCommand(command);
}

void ReceiverStationClient::setAdcNoiceBlankerEnabled(bool state)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_ENABLED);
    command.set_adc_noice_blanker_enebled(state);
    sendCommand(command);
}

void ReceiverStationClient::setAdcNoiceBlankerThreshold(quint16 threshold)
{
    proto::receiver::Command command;
    void *value=&threshold;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_THRESHOLD);
    command.set_adc_noice_blanker_threshold(value,sizeof(threshold));
    sendCommand(command);
}

void ReceiverStationClient::setPreamplifierEnabled(bool state)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_PREAMPLIFIER_ENABLED);
    command.set_preamplifier_enebled(state);
    sendCommand(command);
}

void ReceiverStationClient::setDDC1Frequency(quint32 ddc1Frequency)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_FREQUENCY);
    command.set_ddc1_frequency(ddc1Frequency);
    sendCommand(command);
}

void ReceiverStationClient::setDDC1Type(quint32 typeIndex)
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_TYPE);
    command.set_ddc1_type(typeIndex);
    sendCommand(command);
}

QString ReceiverStationClient::getStationAddress()
{
    return QHostAddress(getPeerAddress().toIPv4Address()).toString();
}

QStringList ReceiverStationClient::getCurrentDeviceSetReceiversNames()
{
    QStringList receiversNames;

    const proto::receiver::DeviceSetInfo &deviceSetInfo=getDeviceSetInfo();
    for(int i=0;i<deviceSetInfo.device_info_size();i++){
        receiversNames<<deviceSetInfo.device_info(i).serial_number().data();
    }
    return receiversNames;
}

//******************* START DDC STREAM ***********************

bool ReceiverStationClient::readDDC1StreamData(Packet &data)
{
    return   d->streamReader->readDDC1StreamData(data);
}

void ReceiverStationClient::startDDC1StreamReader()
{
    if(d->streamReader.get()==nullptr){
        //        d->streamReader.reset(new SignalStreamReader(BUFFER_SIZE,getPeerAddress(),
        //                                                     static_cast<unsigned short>(getDeviceSetInfo().signal_port())));
        d->streamReader.reset(new StreamReader(getPeerAddress(),
                                               8000,//TODO ЗАМЕНИТ НА КОНСТАНТУ
                                               std::make_shared<RingPacketBuffer>(BUFFER_SIZE)));

    }
    //    d->streamReader->startDDC1Stream();
    d->streamReader->start();
}

void ReceiverStationClient::stopDDC1StreamReader()
{
    if(d->streamReader.get()!=nullptr){
        //        d->streamReader->stopDDC1Stream();
        //        d->streamReader->bufferReset();
        d->streamReader->stop();
        d->streamReader->resetBuffer();
    }
}


QHostAddress ReceiverStationClient::getPeerAddress()
{ return d->channel->peerAddress(); }

const proto::receiver::DeviceSetInfo & ReceiverStationClient::getDeviceSetInfo() const
{ return d->info; }


//******************** LOAD FILES *****************************

void ReceiverStationClient::startLoadingFiles(const QStringList &fileNames)
{
    //TODO СДЕЛАТЬ ЗАГРУЗКУ МНОЖЕСТВА FILES
    if(!d->fileLoaders.isEmpty())return;

    qDebug()<<"START LOADING FILES";
    d->fileLoaders<<new FileLoader(getPeerAddress(),8000,fileNames);
    connect(d->fileLoaders.first(),&FileLoader::fullSizeFile,
            this,&ReceiverStationClient::receivedFileSize);

    connect(d->fileLoaders.first(),&FileLoader::bytesProgressFile,
            this,&ReceiverStationClient::bytesProgressFile);

    connect(d->fileLoaders.first(),&FileLoader::finished,
            this,&ReceiverStationClient::stopLoadingFiles);

    d->fileLoaders.first()->start();
}

void ReceiverStationClient::stopLoadingFiles()
{
    d->fileLoaders.first()->stop();
    d->fileLoaders.removeFirst();
    qDebug()<<"STOP LOADING FILES";
}


//********************
struct StreamReader::Impl
{
    Impl(std::shared_ptr<RingPacketBuffer>ddcBuffer):
        streamBuffer(ddcBuffer)
    {}

    QHostAddress address;
    quint16 port;

    std::shared_ptr<RingPacketBuffer>streamBuffer;
    std::atomic<bool> quit;
    QFutureWatcher<void>streamFutureWatcher;
};

StreamReader::StreamReader(const QHostAddress &address, quint16 port,
                           const std::shared_ptr<RingPacketBuffer>&streamBuffer)
    :d(std::make_unique<Impl>(streamBuffer))
{
    d->address=address;
    d->port=port;
    //qDebug("CREATE STREAM DDC READER");
}

bool StreamReader::readDDC1StreamData(Packet &data)
{
    return d->streamBuffer->pop(data);
}

void StreamReader::start()
{
    if(d->streamFutureWatcher.isRunning())return;
    d->quit=false;
    d->streamFutureWatcher.setFuture( QtConcurrent::run(this,&StreamReader::run));
    qDebug("START STREAM READER");
}

void StreamReader::stop()
{
    d->quit=true;
    d->streamFutureWatcher.waitForFinished();
    qDebug("STOP STREAM READER");
}

void StreamReader::resetBuffer()
{
    qDebug()<<"StreamReader::resetBuffer";
    //TODO ПОДОБНЫЙ МЕТОД ЕСТЬ В SHARED_PTR КОТОРЫЙ
    //И ВЫЗЫВАЕТСЯ ИЗ ЗА ЧЕГО ПРОИСХОДИТ БАГ
    d->streamBuffer->reset();
}

void StreamReader::run()
{
    d->quit=false;
    PeerWireClient stream;
    stream.connectToHost(d->address,d->port);
    if(!stream.waitForConnected(1000)){
        qWarning()<<"ERROR connected for"<<d->address<<d->port;
        return;
    }else {
        qDebug()<<"SUCCESS connected for"<<d->address<<d->port;
    }
    //TODO СДЕЛАТЬ ПОТОК
/*
    ConnectRequest request;
    request.set_type(ConnectRequest::CT_STREAM);

    int len=request.ByteSize();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<len;

    stream.writeDataToBuffer(baSize.constData(),baSize.size());
    stream.writeToSocket(baSize.size());

    char buf[request.ByteSize()];
    request.SerializeToArray(buf,request.ByteSize());
    stream.writeDataToBuffer(buf,request.ByteSize());
    stream.writeToSocket(request.ByteSize());
    stream.flush();
    //    qDebug()<<"StreamReader::write request";
    if(stream.waitForReadyRead(5000)){
        char buf[sizeof(int)];
        stream.readFromSocket(sizeof(int));
        stream.readDataFromBuffer(buf,sizeof(int));
        bool ok;
        int num=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        if(!num){
            //            qDebug()<<"*********NUM"<<num;
            return;
        }
        //        qDebug()<<"*****StreamReader::Read";
    }else{
        qDebug("Error Stream Reader");
        return;
    }

    qDebug()<<"THREAD_STREAM_READER********BEGIN DDC STREAM";
    qint64 bytesToRead=200000;
    int blockSize=0;
    int countBytesForRead=0;
    while(!d->quit){
        if(stream.waitForReadyRead(10)||stream.canTransferMore())
        {
            //qDebug()<<blockSize[i];
            if(blockSize==0){
                if(stream.socketBytesAvailable()>=sizeof(int)){
                    stream.readFromSocket(sizeof(int));
                    bool ok;
                    char buf[sizeof(int)];
                    stream.readDataFromBuffer(buf,sizeof(int));
                    blockSize=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
                    countBytesForRead=blockSize;
                    //qDebug()<<"Stream_FS"<<i<<"File Size"<<blockSize[i];
                }
            }
            else
            {
                qint64 available = qMin<qint64>(stream.socketBytesAvailable(), countBytesForRead);
                //qDebug()<<"Aval"<<available;
                if(available>0){
                    qint64 bytes=stream.readFromSocket(qMin<qint64>(available, bytesToRead));
                    if(bytes>0){
                        countBytesForRead-=bytes;
                    }
                }

                if(countBytesForRead==0){
                    char buf[blockSize];
                    stream.readDataFromBuffer(buf,blockSize);
                    Packet packet;
                    packet.ParseFromArray(buf,blockSize);
                    d->streamBuffer->push(packet);
                    qDebug()
                            <<"SD:"<<stream.socketDescriptor()
                           <<"BN:"<<packet.block_number()
                          <<"SR:"<<packet.sample_rate()
                         <<"TOW:"<<packet.time_of_week()
                        <<"DDC_C:"<<packet.ddc_sample_counter()
                       <<"ADC_C"<<packet.adc_period_counter();
                    blockSize=0;
                }
            }
        }
        //        else {
        //            qDebug()<<"ERROR:NO DATA";
        //        }
    }
    stream.disconnectFromHost();
    //stream.deleteLater();
*/
    qDebug()<<"THREAD_STREAM_READER*******END DDC STREAM";
}

//************************** DEL *************************
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
    //static qint64 answerSize=0;// WARNING НЕ РАБОТАЕТ ДЛЯ КОЛИЧЕСТВА СТАНЦИЙ > 1
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

