#include "receiver_station_client.h"

#include "peer_wire_client.h"

#include "stream_server.h"

#include "src/receiver/coh_g35_device_set.h"
#include "src/receiver/device_set_selector.h"
#include "src/receiver/signal_file_writer.h"

#include "command.pb.h"
#include "device_set_info.pb.h"

#include <QDataStream>
//******************ReceiverStationClient***********************

struct ReceiverStationClient::Impl
{
    Impl():commandSocket(std::make_unique<PeerWireClient>()),
        cohG35DeviceSet(std::make_shared<CohG35DeviceSet>()),
        streamServer(cohG35DeviceSet)
    {

    }
    std::unique_ptr<PeerWireClient> commandSocket=nullptr;
    std::shared_ptr<CohG35DeviceSet>cohG35DeviceSet;
    StreamServer streamServer;
//    DeviceSetStreamServer deviceSetStreamServer;
//    FileServer fileServer;
};

ReceiverStationClient::ReceiverStationClient(quint16 signalPort,quint16 filePort):
    d(std::make_unique<Impl>())
{
    connect(d->commandSocket.get(),&PeerWireClient::connected,[this,signalPort,filePort]{
        qDebug()<<"Socket connected"<<d->commandSocket->socketDescriptor();
        sendDeviceSetInfo(signalPort,filePort);

        if(!d->streamServer.isListening()){
            qDebug()<<"*******LISTENING"<<signalPort;
            d->streamServer.listen(QHostAddress(QHostAddress::Any),signalPort);
        }
    });

    connect(d->commandSocket.get(),&PeerWireClient::readyRead,this,&ReceiverStationClient::onReadyRead);
    connect(d->commandSocket.get(),&PeerWireClient::disconnected,this,&ReceiverStationClient::onDisconnected);
}

ReceiverStationClient::~ReceiverStationClient()
{
    d->cohG35DeviceSet->stopDDC1();
}

bool ReceiverStationClient::setupNewDeviceSet(quint32 deviceSetIndex)
{
    ICohG35DDCDeviceSet *deviceSet=DeviceSetSelector::selectDeviceSet(deviceSetIndex);
    if(deviceSet){
        return d->cohG35DeviceSet->setUpDeviceSet(deviceSet);
    }else{
        return false;
    }
}

CohG35DeviceSetSettings ReceiverStationClient::extractSettingsFromCommand(const Command &command)
{
    CohG35DeviceSetSettings settings;
    //settings.powerEnabled=command.power();
    settings.attenuator=command.attenuator();
    settings.preselectors.first=command.preselectors().low_frequency();
    settings.preselectors.second=command.preselectors().high_frequency();
    settings.adcEnabled=command.adc_noice_blanker_enebled();
    const char *thresholdChar=command.adc_noice_blanker_threshold().data();
    const quint16 threshold=*reinterpret_cast<const quint16*>(thresholdChar);
    settings.threshold=threshold;
    settings.preamplifier=command.preamplifier_enebled();
    settings.frequency=command.ddc1_frequency();
    settings.ddcType=command.ddc1_type();
    settings.samplesPerBuffer=command.samples_per_buffer();
    return settings;
}

void ReceiverStationClient::onDisconnected()
{
//    d->deviceSetStreamServer.stopDDC1StreamWriter();
//    d->fileServer.closeFileWriterTasks();

    d->cohG35DeviceSet->stopDDC1();
    emit stationDisconnected();
}

bool ReceiverStationClient::connectToHost(const QHostAddress &address, quint16 port)
{
    //! Если поставить время в метод waitForConnected()
    //! то метод incomingConnection на сторон сервера срабатывает дважды
    //! Если использовать метод waitForConnected то происходит зависание
    //! главного потока при старте второй станции из-за метода waitForFinished
    if(d->commandSocket->state()==QTcpSocket::UnconnectedState){
        d->commandSocket->connectToHost(address,port);
        if(!d->commandSocket->waitForConnected(5000)){
            return false;
        }else {
            return  true;
        }
    }else if(d->commandSocket->state()==QTcpSocket::ConnectingState){
        return false;
    }else if(d->commandSocket->state()==QTcpSocket::ConnectedState){
        qDebug()<<"RECONNECTED"<<d->commandSocket->socketDescriptor();
        return true;
    }else {
        return false;
    }
}

void ReceiverStationClient::sendDeviceSetInfo(quint16 signalPort, quint16 filePort)
{
    COH_G35DDC_DEVICE_SET deviceSetInfos= d->cohG35DeviceSet->getDeviceSetInfo();
    DeviceSetInfo deviceSetInfo;
    for(quint32 index=0;index<deviceSetInfos.DeviceCount;index++){
        DeviceInfo *deviceInfo=deviceSetInfo.add_device_info();
        deviceInfo->set_serial_number(deviceSetInfos.DeviceInfo[index].SerialNumber);
        deviceInfo->set_ddc_type_count(deviceSetInfos.DeviceInfo[index].DDCTypeCount);
        deviceInfo->set_channel_count(deviceSetInfos.DeviceInfo[index].ChannelCount);
        switch (deviceSetInfos.DeviceInfo[index].InterfaceType) {
        case G3XDDC_INTERFACE_TYPE_PCIE:
            deviceInfo->set_interface_type("PCIE");
            break;
        case G3XDDC_INTERFACE_TYPE_USB:
            deviceInfo->set_interface_type("USB");
            break;
        }
    }
    deviceSetInfo.set_signal_port(signalPort);
    deviceSetInfo.set_file_port(filePort);
    qDebug()<<"Signal Port"<<deviceSetInfo.signal_port();
    writeMessage(deviceSetInfo);
}

void ReceiverStationClient::sendStateWorkingCommand(quint32 type,bool state)
{
    Answer answer;
    qDebug()<<"Answer"<<type;
    answer.set_type(static_cast<CommandType>(type));
    answer.set_succesed(state);
    writeMessage(answer);
}

void ReceiverStationClient::writeMessage(const google::protobuf::Message &message)
{
    QByteArray baMessage=serializeMessage(message);
    int byteSize=baMessage.size();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<byteSize;

    qint64 bytesSize=d->commandSocket->writeDataToBuffer(baSize.constData(),baSize.size());
    d->commandSocket->writeToSocket(bytesSize);

    bytesSize=d->commandSocket->writeDataToBuffer(baMessage.constData(),baMessage.size());
    d->commandSocket->writeToSocket(bytesSize);
    d->commandSocket->flush();
}

QByteArray ReceiverStationClient::serializeMessage(const google::protobuf::Message &message)
{
    int  byteSize= message.ByteSize();
//    char bytesArray[byteSize];
    std::vector<char> bytesArray(static_cast<size_t>(byteSize));
    message.SerializeToArray(bytesArray.data(),byteSize);
    QByteArray baMessage(bytesArray.data(),byteSize);///НЕ РАБОТАЕТ С REPEATED DATA ЧАСТЬ ДАННЫХ ОБРЕЗАЕТСЯ
    //qDebug()<<"Size:"<<byteSize<<baMessage.size()<<baMessage.length();
    return baMessage;
}

void ReceiverStationClient::onReadyRead()
{
    bool isGetCommand=false;
    static int size=-1;
    if(size==-1){
        //ПОСТАВИЛ SIZEOF QINT64 - ошибка
        if(d->commandSocket->socketBytesAvailable()>=sizeof(int)){
            d->commandSocket->readFromSocket(sizeof(int));
            char buf[sizeof(int)];
            d->commandSocket->readDataFromBuffer(buf,sizeof(int));
            bool ok;
            size= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
            qDebug()<<"Find Size"<<size;
        }
    }else{
        if(d->commandSocket->socketBytesAvailable()>0){
            qint64 bytes=d->commandSocket->readFromSocket(size);
            if(bytes==size){
                isGetCommand=true;
            }
        }else if(size==0){
            isGetCommand=true;
        }
    }
    if(isGetCommand){
//        char data[size];
        std::vector<char>data(static_cast<size_t>(size));
        d->commandSocket->readDataFromBuffer(data.data(),size);
        parseMessage(QByteArray(data.data(),size));
        size=-1;
    }else {
        onReadyRead();
    }
}

//********************SWITCH COMMAND*******************

void ReceiverStationClient::parseMessage(const QByteArray &baCommand){
    Command command;
    command.ParseFromArray(baCommand.constData(),baCommand.size());
    CommandType type=command.command_type();
    bool succesed=false;
    switch(command.command_type()){
    case CommandType::SET_POWER_OFF:
        succesed=d->cohG35DeviceSet->setPower(false);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_OFF"<<false<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_POWER_ON:
        succesed=d->cohG35DeviceSet->setPower(true);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_ON"<<true<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_SETTINGS:
        succesed=d->cohG35DeviceSet->setSettings(extractSettingsFromCommand(command));
        break;
    case CommandType::START_DDC1:
        qDebug()<<"======Comand  START_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<true;
        d->cohG35DeviceSet->startDDC1(command.samples_per_buffer(),true);
        succesed=true;
        break;
    case CommandType::STOP_DDC1:
        qDebug()<<"======Comand  STOP_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<true;
//        d->deviceSetStreamServer.stopDDC1StreamWriter();
//        d->fileServer.closeFileWriterTasks();
        d->streamServer.stopStreamDDC1();
        d->cohG35DeviceSet->stopDDC1();
        succesed=true;
        break;
    case CommandType::SET_DDC1_TYPE:
        qDebug()<<"======Comand  SET_DDC1_TYPE"<<command.ddc1_type()<<"|| Succesed command"<<true;
        succesed=d->cohG35DeviceSet->setDDC1Type(command.ddc1_type());
        //        cohG35DeviceSet->reStartDdc1(command.ddc1_type(),command.samples_per_buffer(),true);
        break;
    case CommandType::SET_ATTENUATOR:
        succesed=d->cohG35DeviceSet->setAttenuator(command.attenuator());
        qDebug()<<"======Comand  SET_ATTENUATOR "<<command.attenuator()<<"Db"<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_PRESELECTORS:
        succesed=d->cohG35DeviceSet->setPreselectors(command.preselectors().low_frequency(),
                                                  command.preselectors().high_frequency());
        qDebug()<<"======Comand  SET_PRESELECTORS"<<"Frequency:"<<"Low "<<command.preselectors().low_frequency()<<"Hz"
               <<"High "<<command.preselectors().high_frequency()<<"Hz"<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
        succesed=d->cohG35DeviceSet->setAdcNoiceBlankerEnabled(command.adc_noice_blanker_enebled());
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_ENABLED"<<command.adc_noice_blanker_enebled()<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
    {
        const char *thresholdChar=command.adc_noice_blanker_threshold().data();
        const quint16 threshold=*reinterpret_cast<const quint16*>(thresholdChar);
        succesed=d->cohG35DeviceSet->setAdcNoiceBlankerThreshold(threshold);
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD"<<threshold<<"|| Succesed command"<<succesed;
        break;
    }
    case CommandType::SET_PREAMPLIFIER_ENABLED:
        succesed=d->cohG35DeviceSet->setPreamplifierEnabled(command.preamplifier_enebled());
        qDebug()<<"======Comand  SET_PREAMPLIFIED_ENABLED"<<command.preamplifier_enebled()<<"|| Succesed command"<<succesed;
        break;
    case CommandType::SET_DDC1_FREQUENCY:
        succesed=d->cohG35DeviceSet->setDDC1Frequency(command.ddc1_frequency());
        qDebug()<<"======Comand  SET_DDC1_FREQUENCY"<<command.ddc1_frequency()<<"|| Succesed command"<<succesed;
        break;
    case CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
    case CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
        qDebug()<<"Wrong Command Type"<<command.command_type();
        break;
    }

    sendStateWorkingCommand(static_cast<quint32>(type),succesed);
}

//    case CommandType::SET_POWER:
//        if(!command.power()){
//            deviceSetStreamServer.stopDDC1StreamWriter();
//            fileServer.closeFileWriterTasks();
//            cohG35DeviceSet->stopDDC1();
//            sendStateWorkingCommand(CommandType::STOP_DDC1,true);
//        }
//        succesed=cohG35DeviceSet->setPower(command.power());
//        Sleep(100);
//        qDebug()<<"======Comand  SET_POWER"<<command.power()<<"|| Succesed command"<<succesed;
//        break;
//        deviceSetStreamServer.stopDDC1StreamWriter();
//        fileServer.closeFileWriterTasks();
//        cohG35DeviceSet->stopDDC1();
//        sendStateWorkingCommand(CommandType::STOP_DDC1,true);
