#include "receiver_station_client.h"

#include "peer_wire_client.h"

#include "stream_server.h"

#include "src/receiver/coh_g35_device_set.h"
#include "src/receiver/device_set_selector.h"
#include "src/receiver/signal_file_writer.h"

//#include "command.pb.h"
#include "receiver.pb.h"
#include "device_set_info.pb.h"
#include "channel_client.h"

#include <QDataStream>
#include <QTimer>

//******************ReceiverStationClient***********************

struct ReceiverStationClient::Impl
{
    Impl():channel(std::make_unique<ChannelClient>()),
        cohG35DeviceSet(std::make_shared<CohG35DeviceSet>()),
        streamServer(cohG35DeviceSet)
    {

    }
    std::unique_ptr<ChannelClient> channel=nullptr;
    std::shared_ptr<CohG35DeviceSet>cohG35DeviceSet;
    StreamServer streamServer;
};

ReceiverStationClient::ReceiverStationClient(quint16 signalPort,quint16 filePort):
    d(std::make_unique<Impl>())
{
    connect(d->channel.get(),&ChannelClient::connected,[this,signalPort,filePort]{
        qDebug()<<"Socket connected";
        sendDeviceSetInfo(signalPort,filePort);

        if(!d->streamServer.isListening()){
            qDebug()<<"*******LISTENING"<<signalPort;
            d->streamServer.listen(QHostAddress(QHostAddress::Any),signalPort);
        }
    });

    connect(d->channel.get(),&Channel::finished,
            this,&ReceiverStationClient::onDisconnected);

    connect(d->channel.get(),&Channel::messageReceived,
            this,&ReceiverStationClient::onMessageReceived);
}

ReceiverStationClient::~ReceiverStationClient()
{
    d->cohG35DeviceSet->stopDDC1();
}

bool ReceiverStationClient::setupNewDeviceSet(quint32 deviceSetIndex)
{
    ICohG35DDCDeviceSet *deviceSet=DeviceSetSelector::selectDeviceSet(deviceSetIndex);
    //TODO МНОГО IF ELSE
    if(deviceSet){
        return d->cohG35DeviceSet->setUpDeviceSet(deviceSet);
    }else{
        return false;
    }
}

CohG35DeviceSetSettings ReceiverStationClient::extractSettingsFromCommand(
        const proto::receiver::Command &command)
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

    if(d->channel->state()==Channel::NOT_CONNECTED){
        d->channel->connectToHost(address.toString(),port);
        return d->channel->waitChannel(5000);
    }else if(d->channel->state()==Channel::ESTABLISHED){
        qDebug()<<"CHANNEL_ESTABLISHED";
        return true;
    }

    return false;
    //TODO СДЕЛАТЬ ПЕРЕПОДКЛЮЧЕНИЕ
    /*
    if(d->channel->state()==QTcpSocket::UnconnectedState){
        d->channel->connectToHost(address,port);
        if(!d->channel->waitForConnected(5000)){
            return false;
        }else {
            return  true;
        }
    }else if(d->channel->state()==QTcpSocket::ConnectingState){
        return false;
    }else if(d->channel->state()==QTcpSocket::ConnectedState){
        qDebug()<<"RECONNECTED"<<d->channel->socketDescriptor();
        return true;
    }else {
        return false;
    }
    */
}

void ReceiverStationClient::sendDeviceSetInfo(quint16 signalPort, quint16 filePort)
{
    COH_G35DDC_DEVICE_SET deviceSetInfos= d->cohG35DeviceSet->getDeviceSetInfo();
    proto::receiver::DeviceSetInfo *deviceSetInfo=new proto::receiver::DeviceSetInfo;
    for(quint32 index=0;index<deviceSetInfos.DeviceCount;index++){
        proto::receiver::DeviceInfo *deviceInfo=deviceSetInfo->add_device_info();
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
    proto::receiver::HostToClient hostToClient;

    hostToClient.set_allocated_device_set_info(deviceSetInfo);
    qDebug()<<"HostToClient MES"<<hostToClient.ByteSize()
            <<hostToClient.device_set_info().device_info(0).serial_number().data();
//    deviceSetInfo.set_signal_port(signalPort);
//    deviceSetInfo.set_file_port(filePort);
//    qDebug()<<"Signal Port"<<deviceSetInfo.signal_port();
    writeMessage(hostToClient);
}

void ReceiverStationClient::sendStateWorkingCommand(quint32 type,bool state)
{
    proto::receiver::HostToClient hostToClient;

    proto::receiver::Answer answer;
    qDebug()<<"Answer"<<type;
    answer.set_type(static_cast<proto::receiver::CommandType>(type));
    answer.set_succesed(state);

    hostToClient.set_allocated_command_answer(&answer);

    writeMessage(hostToClient);
}

void ReceiverStationClient::writeMessage(const google::protobuf::Message &message)
{
    qDebug()<<"Message MES"<<message.ByteSize();
    d->channel->writeToConnection(serializeMessage(message));
//    QByteArray baMessage=serializeMessage(message);

//    int byteSize=baMessage.size();
//    QByteArray baSize;
//    QDataStream out(&baSize,QIODevice::WriteOnly);
//    out<<byteSize;

//    qint64 bytesSize=d->channel->writeDataToBuffer(baSize.constData(),baSize.size());
//    d->channel->writeToSocket(bytesSize);

//    bytesSize=d->channel->writeDataToBuffer(baMessage.constData(),baMessage.size());
//    d->channel->writeToSocket(bytesSize);
//    d->channel->flush();
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

void ReceiverStationClient::onMessageReceived(const QByteArray &buffer)
{
    proto::receiver::ClientToHost clientToHost;
    if(!clientToHost.ParseFromArray(buffer.constData(),buffer.size())){
        qDebug()<<"ERROR PARSE CLIENT_TO_HOST MES";
        return;
    }

    if(clientToHost.has_command()){
        readCommanPacket(clientToHost.command());
    }else qDebug()<<"ERROR CLIENT_TO_HOST_MES IS EMPTY";
}

//********************SWITCH COMMAND*******************

void ReceiverStationClient::readCommanPacket(const proto::receiver::Command &command){
    proto::receiver::CommandType type=command.command_type();
    bool succesed=false;
    switch(command.command_type()){
    case proto::receiver::CommandType::SET_POWER_OFF:
        succesed=d->cohG35DeviceSet->setPower(false);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_OFF"<<false<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_POWER_ON:
        succesed=d->cohG35DeviceSet->setPower(true);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_ON"<<true<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_SETTINGS:
        succesed=d->cohG35DeviceSet->setSettings(extractSettingsFromCommand(command));
        break;
    case proto::receiver::CommandType::START_DDC1:
        qDebug()<<"======Comand  START_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<true;
        d->cohG35DeviceSet->startDDC1(command.samples_per_buffer(),true);
        succesed=true;
        break;
    case proto::receiver::CommandType::STOP_DDC1:
        qDebug()<<"======Comand  STOP_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<true;
//        d->deviceSetStreamServer.stopDDC1StreamWriter();
//        d->fileServer.closeFileWriterTasks();
        d->streamServer.stopStreamDDC1();
        d->cohG35DeviceSet->stopDDC1();
        succesed=true;
        break;
    case proto::receiver::CommandType::SET_DDC1_TYPE:
        qDebug()<<"======Comand  SET_DDC1_TYPE"<<command.ddc1_type()<<"|| Succesed command"<<true;
        succesed=d->cohG35DeviceSet->setDDC1Type(command.ddc1_type());
        //        cohG35DeviceSet->reStartDdc1(command.ddc1_type(),command.samples_per_buffer(),true);
        break;
    case proto::receiver::CommandType::SET_ATTENUATOR:
        succesed=d->cohG35DeviceSet->setAttenuator(command.attenuator());
        qDebug()<<"======Comand  SET_ATTENUATOR "<<command.attenuator()<<"Db"<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_PRESELECTORS:
        succesed=d->cohG35DeviceSet->setPreselectors(command.preselectors().low_frequency(),
                                                  command.preselectors().high_frequency());
        qDebug()<<"======Comand  SET_PRESELECTORS"<<"Frequency:"<<"Low "<<command.preselectors().low_frequency()<<"Hz"
               <<"High "<<command.preselectors().high_frequency()<<"Hz"<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
        succesed=d->cohG35DeviceSet->setAdcNoiceBlankerEnabled(command.adc_noice_blanker_enebled());
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_ENABLED"<<command.adc_noice_blanker_enebled()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
    {
        const char *thresholdChar=command.adc_noice_blanker_threshold().data();
        const quint16 threshold=*reinterpret_cast<const quint16*>(thresholdChar);
        succesed=d->cohG35DeviceSet->setAdcNoiceBlankerThreshold(threshold);
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD"<<threshold<<"|| Succesed command"<<succesed;
        break;
    }
    case proto::receiver::CommandType::SET_PREAMPLIFIER_ENABLED:
        succesed=d->cohG35DeviceSet->setPreamplifierEnabled(command.preamplifier_enebled());
        qDebug()<<"======Comand  SET_PREAMPLIFIED_ENABLED"<<command.preamplifier_enebled()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_DDC1_FREQUENCY:
        succesed=d->cohG35DeviceSet->setDDC1Frequency(command.ddc1_frequency());
        qDebug()<<"======Comand  SET_DDC1_FREQUENCY"<<command.ddc1_frequency()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
    case proto::receiver::CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
        qDebug()<<"Wrong Command Type"<<command.command_type();
        break;
    }

    sendStateWorkingCommand(static_cast<quint32>(type),succesed);
}
