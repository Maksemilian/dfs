#include "host_ds.h"

#include "wrd_coh_g35_ds.h"
#include "wrd_ds_selector.h"

#include "channel_host.h"
//******************ReceiverStationClient***********************

struct DeviceSetClient::Impl
{
    Impl(net::ChannelHost *channel):
        channel(channel)
    {}

    Impl(net::ChannelHost *channel,
         const std::shared_ptr<CohG35DeviceSet>&deviceSet):
        channel(channel),cohG35DeviceSet(deviceSet)
    {}

    std::unique_ptr<net::ChannelHost> channel;
    std::shared_ptr<CohG35DeviceSet>cohG35DeviceSet;
};

DeviceSetClient::DeviceSetClient(net::ChannelHost*channelHost)
    :d(std::make_unique<Impl>(channelHost))
{
    qDebug()<<"Create ReceiverStationClient";

    connect(d->channel.get(),&net::Channel::finished,
            this,&DeviceSetClient::onDisconnected);

    connect(d->channel.get(),&net::Channel::messageReceived,
            this,&DeviceSetClient::onMessageReceived);
}

DeviceSetClient::DeviceSetClient(net::ChannelHost*channelHost,
                                 const std::shared_ptr<CohG35DeviceSet>&deviceSet)
    : d(std::make_unique<Impl>(channelHost,deviceSet))
{
    qDebug()<<"Create ReceiverStationClient";

    connect(d->channel.get(),&net::Channel::finished,
            this,&DeviceSetClient::onDisconnected);

    connect(d->channel.get(),&net::Channel::messageReceived,
            this,&DeviceSetClient::onMessageReceived);
}

DeviceSetClient::~DeviceSetClient()
{
    d->cohG35DeviceSet->stopDDC1();
}

void DeviceSetClient::setCohDeviceSet(const std::shared_ptr<CohG35DeviceSet> &shPtrCohG35DeviceSet)
{
    d->cohG35DeviceSet=shPtrCohG35DeviceSet;
}

const std::shared_ptr<CohG35DeviceSet> &DeviceSetClient::getCohDeviceSet()
{
    return d->cohG35DeviceSet;
}
//std::shared_ptr<RingBuffer<proto::receiver::Packet>> DeviceSetClient::getBuffer()
//{
//    return  d->cohG35DeviceSet->getBuffer();
//}

QByteArray DeviceSetClient::serializeMessage(const google::protobuf::Message &message)
{
    int  byteSize= message.ByteSize();
    //    char bytesArray[byteSize];
    std::vector<char> bytesArray(static_cast<size_t>(byteSize));
    message.SerializeToArray(bytesArray.data(),byteSize);
    QByteArray baMessage(bytesArray.data(),byteSize);///НЕ РАБОТАЕТ С REPEATED DATA ЧАСТЬ ДАННЫХ ОБРЕЗАЕТСЯ
    //qDebug()<<"Size:"<<byteSize<<baMessage.size()<<baMessage.length();
    return baMessage;
}


DeviceSetSettings DeviceSetClient::extractSettingsFromCommand(
        const proto::receiver::Command &command)
{
    DeviceSetSettings settings;
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

void DeviceSetClient::onDisconnected()
{
    d->cohG35DeviceSet->stopDDC1();
    emit stationDisconnected();
}

void DeviceSetClient::sendDeviceSetInfo()
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
    writeMessage(hostToClient);
}

void DeviceSetClient::sendCommandAnswer( proto::receiver::Answer *commandAnswer)
{
    proto::receiver::HostToClient hostToClient;

    qDebug()<<"Answer"<<commandAnswer->type()<<commandAnswer->succesed();
    hostToClient.set_allocated_command_answer(commandAnswer);
    writeMessage(hostToClient);
}

void DeviceSetClient::writeMessage(const google::protobuf::Message &message)
{
    qDebug()<<"Message MES"<<message.ByteSize();
    d->channel->writeToConnection(serializeMessage(message));
}

void DeviceSetClient::onMessageReceived(const QByteArray &buffer)
{
    qDebug()<<"Message Received";
    proto::receiver::ClientToHost clientToHost;
    if(!clientToHost.ParseFromArray(buffer.constData(),buffer.size())){
        qDebug()<<"ERROR PARSE CLIENT_TO_HOST MES";
        return;
    }

    if(clientToHost.has_command()){
        readCommandPacket(clientToHost.command());
    }else qDebug()<<"ERROR CLIENT_TO_HOST_MES IS EMPTY";
}

//********************SWITCH COMMAND*******************

void DeviceSetClient::readCommandPacket(const proto::receiver::Command &command){
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
        //TODO RETURN VALUE
        succesed=true;
        break;
    case proto::receiver::CommandType::STOP_DDC1:
        qDebug()<<"======Comand  STOP_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<true;
        d->cohG35DeviceSet->stopDDC1();
        //TODO RETURN VALUE
        succesed=true;
        break;
    case proto::receiver::CommandType::SET_DDC1_TYPE:
        qDebug()<<"======Comand  SET_DDC1_TYPE"<<command.ddc1_type()<<"|| Succesed command"<<true;
        succesed=d->cohG35DeviceSet->setDDC1Type(command.ddc1_type());
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
    case proto::receiver::CommandType::SET_SHIFT_PHASE_DDC:
        succesed=d->cohG35DeviceSet->setShiftPhaseDDC1(command.shift_phase_ddc1().device_index(),
                                                       command.shift_phase_ddc1().phase_shift());
        qDebug()<<"======Comand  SET_DDC1_SHIFT_PHASE"<<command.shift_phase_ddc1().device_index()
               <<command.shift_phase_ddc1().phase_shift()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_DEVICE_SET_INDEX:
        d->cohG35DeviceSet=DeviceSetSelector::selectDeviceSet(command.device_set_index());
        d->cohG35DeviceSet.get()!=nullptr? succesed=true:succesed=false;
        if(succesed){
            qDebug()<<"======Comand  SET_DEVICE_SET_INDEX"
                   <<d->cohG35DeviceSet->getDeviceSetName();
            sendDeviceSetInfo();
        }
        break;
    case proto::receiver::UNKNOWN_COMMAND:
        //TODO
        qDebug()<<"====== UNKMOWN COMMAND";
        break;
    case proto::receiver::CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
    case proto::receiver::CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
        qDebug()<<"Wrong Command Type"<<command.command_type();
        break;
    }
    proto::receiver::Answer *answer=new proto::receiver::Answer;
    answer->set_type(command.command_type());
    answer->set_succesed(succesed);
    sendCommandAnswer(answer);
}
