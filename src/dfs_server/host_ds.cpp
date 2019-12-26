#include "host_ds.h"

#include "channel_host.h"
#include "wrd_coh_g35_ds.h"
#include "wrd_d_g35.h"

#include "wrd_device_selector.h"

#include "receiver.pb.h"
#include "ring_buffer.h"

#include <QDataStream>
#include <QThread>
#include <QHostAddress>
//******************ReceiverStationClient***********************


QByteArray serializeMessage(const google::protobuf::Message &message)
{
    int  byteSize= message.ByteSize();
    //    char bytesArray[byteSize];
    std::vector<char> bytesArray(static_cast<size_t>(byteSize));
    message.SerializeToArray(bytesArray.data(),byteSize);
    QByteArray baMessage(bytesArray.data(),byteSize);///НЕ РАБОТАЕТ С REPEATED DATA ЧАСТЬ ДАННЫХ ОБРЕЗАЕТСЯ
    //qDebug()<<"Size:"<<byteSize<<baMessage.size()<<baMessage.length();
    return baMessage;
}

struct DeviceSetClient::Impl
{
    enum class StreamType {ST_DDC1};

    Impl(net::ChannelHost *channel):
        channel(channel),
        buffer(std::make_shared<RingBuffer<proto::receiver::Packet>>(16)),
        //        deviceCreator(std::make_unique<CohG35DeviceCreator>()),
        deviceMode(proto::receiver::DM_COHERENT)
    {}

    Impl(net::ChannelHost *channel,
         const std::shared_ptr<IDevice>&deviceSet):
        channel(channel),
        device(deviceSet),
        buffer(std::make_shared<RingBuffer<proto::receiver::Packet>>(16)),
        //        deviceCreator(std::make_unique<CohG35DeviceCreator>()),
        deviceMode(proto::receiver::DM_COHERENT)
    {}

    std::unique_ptr<net::ChannelHost> channel;
    std::shared_ptr<IDevice> device;
    ShPtrRingBuffer buffer;
    //    std::unique_ptr<DeviceCreator>deviceCreator;
    proto::receiver::DeviceMode deviceMode;
    SignalStreamWriter *streamDDC1=nullptr;
};

DeviceSetClient::DeviceSetClient(net::ChannelHost*channelHost)
    :d(std::make_unique<Impl>(channelHost))
{
    qDebug()<<"Create DeviceSetClient";

    connect(d->channel.get(),&net::Channel::finished,
            this,&DeviceSetClient::onDisconnected);

    connect(d->channel.get(),&net::Channel::messageReceived,
            this,&DeviceSetClient::onMessageReceived);
}

DeviceSetClient::~DeviceSetClient()
{
    d->device->stopDDC1();

    if(d->streamDDC1)d->streamDDC1->stop();
    qDebug()<<"DESTR DeviceSetCleint";
}

ShPtrRingBuffer DeviceSetClient::ddc1Buffer()
{
    return d->buffer;
}

DeviceSettings DeviceSetClient::extractSettingsFromCommand(
        const proto::receiver::Command &command)
{
    DeviceSettings settings;
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
    d->device->stopDDC1();
    emit deviceDisconnected();
}

void DeviceSetClient::sendDeviceSetInfo()
{
    if(dynamic_cast<CohG35DeviceSet*>(d->device.get())){
        setDeviceInfoCoherent();
    }else if(dynamic_cast<G35Device*>(d->device.get())){
        sendDeviceInfoSingle();
    }else {
        qDebug()<<"BAD CAST";
    }
}

void DeviceSetClient::sendDeviceInfoSingle()
{
    G35DDC_DEVICE_INFO deviceInf= dynamic_cast<G35Device*>(d->device.get())->getDeviceInfo();
    proto::receiver::DeviceSetInfo *deviceSetInfo=new proto::receiver::DeviceSetInfo;
    proto::receiver::DeviceInfo *deviceInfo=deviceSetInfo->add_device_info();
    deviceInfo->set_serial_number(deviceInf.SerialNumber);
    deviceInfo->set_ddc_type_count(deviceInf.DDCTypeCount);
    deviceInfo->set_channel_count(deviceInf.ChannelCount);

    switch (deviceInf.InterfaceType) {
    case G3XDDC_INTERFACE_TYPE_PCIE:
        deviceInfo->set_interface_type("PCIE");
        break;
    case G3XDDC_INTERFACE_TYPE_USB:
        deviceInfo->set_interface_type("USB");
        break;
    }
    proto::receiver::HostToClient hostToClient;

    hostToClient.set_allocated_device_set_info(deviceSetInfo);
    qDebug()<<"HostToClient MES"<<hostToClient.ByteSize()
           <<hostToClient.device_set_info().device_info(0).serial_number().data();
    writeMessage(hostToClient);
}

void DeviceSetClient::setDeviceInfoCoherent()
{

    COH_G35DDC_DEVICE_SET deviceSetInfos= dynamic_cast<CohG35DeviceSet*>(d->device.get())->getDeviceSetInfo();
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

void DeviceSetClient::sendDevieSetStatus()
{
    proto::receiver::HostToClient hostToClient;
    hostToClient.set_is_ready(true);
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
        succesed=d->device->setPower(false);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_OFF"<<false<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_POWER_ON:
        succesed=d->device->setPower(true);
        Sleep(SLEEP_TIME);
        qDebug()<<"======Comand  SET_POWER_ON"<<true<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_SETTINGS:
        succesed=d->device->setSettings(extractSettingsFromCommand(command));
        break;
    case proto::receiver::CommandType::START_DDC1:
        succesed=d->device->startDDC1(command.samples_per_buffer());
        qDebug()<<"======Comand  START_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::STOP_DDC1:
        succesed=d->device->stopDDC1();
        qDebug()<<"======Comand  STOP_DDC1"<<command.samples_per_buffer()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_DDC1_TYPE:
        succesed=d->device->setDDC1Type(command.ddc1_type());
        qDebug()<<"======Comand  SET_DDC1_TYPE"<<command.ddc1_type()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_ATTENUATOR:
        succesed=d->device->setAttenuator(command.attenuator());
        qDebug()<<"======Comand  SET_ATTENUATOR "<<command.attenuator()<<"Db"<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_PRESELECTORS:
        succesed=d->device->setPreselectors(command.preselectors().low_frequency(),
                                            command.preselectors().high_frequency());
        qDebug()<<"======Comand  SET_PRESELECTORS"<<"Frequency:"<<"Low "<<command.preselectors().low_frequency()<<"Hz"
               <<"High "<<command.preselectors().high_frequency()<<"Hz"<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
        succesed=d->device->setAdcNoiceBlankerEnabled(command.adc_noice_blanker_enebled());
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_ENABLED"<<command.adc_noice_blanker_enebled()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
    {
        const char *thresholdChar=command.adc_noice_blanker_threshold().data();
        const quint16 threshold=*reinterpret_cast<const quint16*>(thresholdChar);
        succesed=d->device->setAdcNoiceBlankerThreshold(threshold);
        qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD"<<threshold<<"|| Succesed command"<<succesed;
        break;
    }
    case proto::receiver::CommandType::SET_PREAMPLIFIER_ENABLED:
        succesed=d->device->setPreamplifierEnabled(command.preamplifier_enebled());
        qDebug()<<"======Comand  SET_PREAMPLIFIED_ENABLED"<<command.preamplifier_enebled()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_DDC1_FREQUENCY:
        succesed=d->device->setDDC1Frequency(command.ddc1_frequency());

        qDebug()<<"======Comand  SET_DDC1_FREQUENCY"<<command.ddc1_frequency()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_SHIFT_PHASE_DDC:
        //        succesed=d->cohG35DeviceSet->setShiftPhaseDDC1(command.shift_phase_ddc1().device_index(),
        //                                                       command.shift_phase_ddc1().phase_shift());
        qDebug()<<"======Comand  SET_DDC1_SHIFT_PHASE UNUSED"<<command.shift_phase_ddc1().device_index()
               <<command.shift_phase_ddc1().phase_shift()<<"|| Succesed command"<<succesed;
        break;
    case proto::receiver::CommandType::SET_DEVICE_INDEX:
        if (d->deviceMode==proto::receiver::DM_COHERENT)
        {
            d->device = DeviceFactory::createCohG35Device(command.device_set_index(),
                                                          d->buffer,
                                                          command.demo_mode());

        }
        else if(d->deviceMode==proto::receiver::DM_SINGLE)
        {
            d->device = DeviceFactory::createSingleG35Device(command.device_set_index(),
                                                             d->buffer,
                                                             command.demo_mode());
        }
        if(d->device.get()){
            succesed=true;
            sendDeviceSetInfo();
            qDebug()<<"======Comand  SET_DEVICE_INDEX"<<d->deviceMode;
        }
        else{
            succesed=false;
            qDebug()<<"======Comand  SET_DEVICE_INDEX FAILED"<<d->deviceMode;
        }
        break;
    case proto::receiver::SET_DEVICE_MODE:
        d->deviceMode = command.device_mode();
        succesed = true;
        if( d->deviceMode == proto::receiver::DM_UNKNOWN)
        {
            qDebug()<<"====== SET MODE FAILED";
            succesed=false;
        };
        break;
    case proto::receiver::START_SENDING_DDC1_STREAM:
        qDebug()<<"===== START_SENDING_DDC1_STREAM";
        startSendingDDC1Stream(d->channel->peerAddress(),
                               static_cast<quint16>(command.stream_port()),
                               d->buffer);
        succesed=true;
        break;
    case proto::receiver::STOP_SENDING_DDC1_STREAM:
        qDebug()<<"===== STOP_SENDING_DDC1_STREAM";
        d->streamDDC1->stop();
        succesed=true;
        break;

    case proto::receiver::UNKNOWN_COMMAND:
    case proto::receiver::CommandType_INT_MAX_SENTINEL_DO_NOT_USE_:
    case proto::receiver::CommandType_INT_MIN_SENTINEL_DO_NOT_USE_:
        qDebug()<<"====== UNKMOWN COMMAND"<<command.command_type();
        break;
    }
    proto::receiver::Answer *answer=new proto::receiver::Answer;
    answer->set_type(command.command_type());
    answer->set_succesed(succesed);
    sendCommandAnswer(answer);
}

void DeviceSetClient::startSendingDDC1Stream(const QHostAddress &address,quint16 port,
                                             const ShPtrRingBuffer &buffer)
{
    QThread *thread=new QThread;

    d->streamDDC1=new SignalStreamWriter(address,port,buffer);
    d->streamDDC1->moveToThread(thread);

    connect(thread,&QThread::started,
            d->streamDDC1,&SignalStreamWriter::start);

    connect(d->streamDDC1,&SignalStreamWriter::finished,
            thread,&QThread::quit);

    connect(thread,&QThread::finished,
            d->streamDDC1,&SignalStreamWriter::deleteLater);

    connect(thread,&QThread::destroyed,
            thread,&QThread::deleteLater);

    thread->start();
}

//************************ STREAM DDC1 **********************

SignalStreamWriter::SignalStreamWriter(const QHostAddress &address,quint16 port,
                                       const ShPtrRingBuffer &buffer)
    :  _address(address),_port(port),_buffer(buffer){}

void SignalStreamWriter::process()
{
    //    qDebug()<<"********PROCESS:"<<QHostAddress(_address.toIPv4Address()).toString()
    //           <<_port;
    _streamSocket.reset(new net::ChannelClient);

    connect(_streamSocket.get(),&net::ChannelClient::connected,
            [this]{

        qDebug("SignalStreamWriter::BEGIN PROCESS STREAM");

        _quit=false;
        //********************
        proto::receiver::Packet packet;
        proto::receiver::ClientToHost clientToHost;
        while(!_quit){
            if(_buffer->pop(packet))
            {
                clientToHost.mutable_packet()->CopyFrom(packet);
                int packetByteSize= clientToHost.ByteSize();

                QByteArray ba;
                QDataStream out(&ba,QIODevice::WriteOnly);

                out<<packetByteSize;
                if(!_streamSocket->isOpen()){
                    qDebug("Socket Close");
                    break;
                }

                _streamSocket->writeDataToBuffer(ba.constData(),ba.size());
                _streamSocket->writeToSocket(ba.size());
                _streamSocket->flush();
                char buf[packetByteSize];
                clientToHost.SerializeToArray(buf,packetByteSize);

                _streamSocket->writeDataToBuffer(buf,packetByteSize);

                qint64 bytesWriten=0;
                while (!_quit&&bytesWriten != packetByteSize ){
                    if(!_streamSocket->isWritable()){
                        _quit=true;
                        break;
                    }
                    qint64 bytes=_streamSocket->writeToSocket(packetByteSize);
                    if(bytes==-1){
                        _quit=true;
                        break;
                    }
                    bytesWriten+=bytes;
                }
                qDebug()<<"WRITE:"
                       <<packet.block_number()
                      <<packet.sample_rate()
                     <<"TOW:"<<packet.time_of_week()
                    <<"DDC_C"<<packet.ddc_sample_counter()
                   <<"ADC_C"<<packet.adc_period_counter()
                  <<clientToHost.ByteSize();
            }
        }
        _streamSocket->disconnectFromHost();
        //    _streamSocket->waitForDisconnected(5000);
        emit finished();
        qDebug("SignalStreamWriter::END PROCESS WRITER");
    });
    _streamSocket->connectToHost(QHostAddress(_address.toIPv4Address()).toString()
                                 ,_port,SessionType::SESSION_SIGNAL_STREAM);

    _streamSocket->waitForConnected(5000);
}

void SignalStreamWriter::start()
{
    _quit=false;
    process();
    qDebug("START STREAM WRITER");
}

void SignalStreamWriter::stop()
{
    _quit=true;
    qDebug("STOP STREAM WRITER");
}
