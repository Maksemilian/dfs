#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "channel_host.h"
#include "channel_client.h"

#include "wrd_coh_g35_ds.h"

#include <memory>
#include <QObject>
#include <QFutureWatcher>
#include <QHostAddress>

using ShPtrRingBuffer=std::shared_ptr<RingBuffer<proto::receiver::Packet>>;
class DeviceSetClient:public QObject
{
    Q_OBJECT

    static const int SLEEP_TIME=100;
public:
    DeviceSetClient(net::ChannelHost*channelHost);
    DeviceSetClient(net::ChannelHost*channelHost,
                    const std::shared_ptr<CohG35DeviceSet>&deviceSet);
    ~DeviceSetClient();

//    void setCohDeviceSet(const std::shared_ptr<CohG35DeviceSet>&shPtrCohG35DeviceSet);
//    const std::shared_ptr<CohG35DeviceSet> &getCohDeviceSet();
    ShPtrRingBuffer ddc1Buffer();
signals:
    void stationDisconnected();
    void changedDeviceSet(unsigned int indexDeviceSet);
private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray &buffer);
private:
    void sendDeviceSetInfo();
    void sendDeviceInfoSingle();
    void setDeviceInfoCoherent();
    void sendCommandAnswer(proto::receiver::Answer *commandAnswer);
    void readCommandPacket(const proto::receiver::Command &command);
    DeviceSettings extractSettingsFromCommand(const proto::receiver::Command &command);

//    QByteArray serializeMessage(const google::protobuf::Message &message);
    void writeMessage(const google::protobuf::Message &message);
    void createThread(const QHostAddress &address,quint16 port,
                                       const ShPtrRingBuffer &buffer);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

class StreamDDC1T:public QObject
{
    Q_OBJECT
public:
    StreamDDC1T(const QHostAddress &address,quint16 port,
            ShPtrRingBuffer _buffer);

    ~StreamDDC1T(){qDebug()<<"DESTR_DDC1";}
    void start();
    void stop();
signals:
    void finished();
    void next();
private:
    void process();
private:
    std::unique_ptr<net::ChannelClient>_streamSocket;
    QHostAddress _address;
    quint16 _port;
    ShPtrRingBuffer _buffer;
    std::atomic<bool>_quit;
    QFutureWatcher<void> _fw;
};
#endif // RECEIVER_STATION_CLIENT_H
