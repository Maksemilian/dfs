#include "client_ds.h"

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
};

/*! \addtogroup client
 */
///@{

DeviceClient::DeviceClient(const ConnectData& connectData, QObject* parent)
    :    Client (connectData, parent),
         d(std::make_unique<Impl>())
{
}

void DeviceClient::sendCommand(proto::receiver::Command& command)
{
    d->commandQueue.enqueue(command.command_type());
    proto::receiver::ClientToHost clientToHost;
    clientToHost.mutable_command()->CopyFrom(command);
    // d->channel->writeToConnection(serializeMessage(clientToHost));
    sendMessage(clientToHost);
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

DeviceClient::~DeviceClient()
{
}

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
        showCommandAnswer(answer.type());
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

//*************** MSG ***************
void DeviceClient::showCommandAnswer(proto::receiver::CommandType commandType)
{
    switch (commandType)
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
//                emit ddc1StreamStarted();
            break;
        case proto::receiver::CommandType::STOP_DDC1:
            qDebug() << "STOPED_DDC1:";
//                emit ddc1StreamStoped();
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
}

//TODO сервер должен отсылать сообщение с ошибкой
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
///@}

//********************* STREAM SERVER *****************

//#include "ring_buffer.h"
//#include "receiver.pb.h"

//#include <QThread>

///*! \addtogroup client
// */
/////@{
//SignalStreamServer::SignalStreamServer(quint8 bufferSize)
//{
//    Q_UNUSED(bufferSize);

//    buffers[StreamType::ST_DDC1] =
//        std::make_shared<RadioChannel>();
//}

//ShPtrRadioChannel SignalStreamServer::getChannel(StreamType type)
//{
//    if(type == StreamType::ST_DDC1)
//    {
//        return buffers[StreamType::ST_DDC1];
//    }
//    return nullptr;
//}

//void SignalStreamServer::createSession(net::ChannelHost* channelHost)
//{
//    if(channelHost->sessionType() == SessionType::SESSION_SIGNAL_STREAM)
//    {
//        qDebug() << "STREAM SESSION";
//        createThread(channelHost);
//    }
//    else
//    {
//        qDebug() << "ERROR SESSION TYPE";
//    }
//}

//void SignalStreamServer::createThread(net::ChannelHost* channelHost)
//{
//    QThread* thread = new QThread;

//    SignalStreamReader*   streamDDC1 =
//        new SignalStreamReader(channelHost,
//                               buffers[StreamType::ST_DDC1]->inBuffer());
//    streamDDC1->moveToThread(thread);
//    channelHost->moveToThread(thread);

//    connect(thread, &QThread::started,
//            streamDDC1, &SignalStreamReader::process);

//    connect(streamDDC1, &SignalStreamReader::finished,
//            thread, &QThread::quit);

//    connect(thread, &QThread::finished,
//            streamDDC1, &SignalStreamReader::deleteLater);

//    connect(thread, &QThread::destroyed,
//            thread, &QThread::deleteLater);

//    thread->start();
//}

////************************** SIGNAL STREM READER **************************

//struct SignalStreamReader::Impl
//{
//    Impl(net::ChannelHost* channelHost,
//         std::shared_ptr<RingBuffer<proto::receiver::Packet>>ddcBuffer):
//        stream(channelHost),
//        streamBuffer(ddcBuffer),
//        quit(true)
//    {
//    }

//    std::unique_ptr<net::ChannelHost> stream;
//    std::shared_ptr<RingBuffer<proto::receiver::Packet>>streamBuffer;
//    std::atomic<bool> quit;
//    qint32 key;
//};

//SignalStreamReader::SignalStreamReader(net::ChannelHost* channelHost,
//                                       const ShPtrPacketBuffer streamBuffer)
//    : d(std::make_unique<Impl>(channelHost, streamBuffer))
//{

//}

//SignalStreamReader::~SignalStreamReader()
//{
//    qDebug() << "STOP Stream Reader DDC1";
//    qDebug() << "DESTR::StreamReader";
//}


//void SignalStreamReader::process()
//{
//    qDebug() << "START Stream Reader DDC1";
//    connect(d->stream.get(), &net::ChannelHost::messageReceived,
//            this, &SignalStreamReader::onMessageReceive);

//    connect(d->stream.get(), &net::ChannelHost::finished,
//            this, &SignalStreamReader::finished);
//}

//void SignalStreamReader::onMessageReceive(const QByteArray& buffer)
//{
//    proto::receiver::ClientToHost clientToHost;

//    if(!clientToHost.ParseFromArray(buffer.constData(), buffer.size()))
//    {
//        qDebug() << "ERROR PARSE STREAM_READER HOST TO CLIENT";
//        return;
//    }

//    if(clientToHost.has_packet())
//    {
//        d->streamBuffer->push(const_cast<proto::receiver::Packet&>
//                              (clientToHost.packet()));
//        qDebug()
//                << "BN:" << clientToHost.packet().block_number()
//                << "SR:" << clientToHost.packet().sample_rate()
//                << "TOW:" << clientToHost.packet().time_of_week()
//                << "DDC_C:" << clientToHost.packet().ddc_sample_counter()
//                << "ADC_C" << clientToHost.packet().adc_period_counter();
//    }
//}
///@}
