#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "channel_host.h"
#include "channel_client.h"

#include "wrd_coh_g35_ds.h"

#include <memory>

#include <QObject>
#include <QHostAddress>

using ShPtrRingBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class DeviceClient: public QObject
{
    Q_OBJECT

    static const int SLEEP_TIME = 100;
  public:
    DeviceClient(net::ChannelHost* channelHost);

    ~DeviceClient();

    ShPtrRingBuffer ddc1Buffer();
    void sendDevieSetStatus();
    void sendDeviceSetInfo();
  signals:
    void deviceDisconnected();
    void changedDeviceSet(unsigned int indexDeviceSet);
  private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray& buffer);
  private:
    void sendDeviceInfoSingle();
    void setDeviceInfoCoherent();
    void sendCommandAnswer(proto::receiver::Answer* commandAnswer);
    void readCommandPacket(const proto::receiver::Command& command);
    DeviceSettings extractSettingsFromCommand(const proto::receiver::Command& command);

    void writeMessage(const google::protobuf::Message& message);
    void startSendingDDC1Stream(const QHostAddress& address, quint16 port,
                                const ShPtrRingBuffer& buffer);
  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

class SignalStreamWriter: public QObject
{
    Q_OBJECT
  public:
    SignalStreamWriter(const QHostAddress& address, quint16 port,
                       const ShPtrRingBuffer& _buffer);

    ~SignalStreamWriter()
    {
        qDebug() << "DESTR_DDC1";
    }
    void start();
    void stop();
  signals:
    void finished();
  private:
    void process();
  private:
    std::unique_ptr<net::ChannelClient>_streamSocket;
    QHostAddress _address;
    quint16 _port;
    ShPtrRingBuffer _buffer;
    std::atomic<bool>_quit;
};
#endif // RECEIVER_STATION_CLIENT_H
