#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "receiver.pb.h"
#include "ring_buffer.h"

#include "client.h"

#include <QObject>

using ShPtrPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class QHostAddress;

class DeviceSetClient: public Client
{
    Q_OBJECT
  public:
    DeviceSetClient(const ConnectData& connectData, QObject* parent = nullptr);
    ~DeviceSetClient();

    void sendCommand(proto::receiver::Command& command);

    void setLiceningStreamPort(quint16 port);
    quint16 liceningStreamPort();
    const proto::receiver::DeviceSetInfo& getDeviceSetInfo()const;
    QString getCurrentDeviceSetName()const;

    QStringList receiverNameList()const;
    ShPtrPacketBuffer getDDC1Buffer()const;
  signals:
    void commandSuccessed();
    void commandFailed(const QString& errorString);

    void deviceReady(const QStringList& receivers);
    void deviceInfoUpdated(const QStringList& receivers);

    void ddc1StreamStarted();
    void ddc1StreamStoped();

  private:
    void readAnswerPacket(const proto::receiver::Answer& answer);
    QString errorString(proto::receiver::CommandType commandType);
  private slots:
    void onMessageReceived(const QByteArray& buffer);
  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

using ShPtrDeviceClient = std::shared_ptr<DeviceSetClient>;
#endif // RECEIVER_STATION_CLIENT_H
