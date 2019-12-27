#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "receiver.pb.h"
#include "ring_buffer.h"

#include <QObject>

using ShPtrPacketBuffer =std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class QHostAddress;

class DeviceSetClient: public QObject
{
    Q_OBJECT
public:
    DeviceSetClient(QObject *parent=nullptr);
    ~DeviceSetClient();

    void connectToHost(const QHostAddress &address,quint16 port);
    void disconnectFromHost();
    void sendCommand(proto::receiver::Command &command);
    void setLiceningStreamPort(quint16 port);
    quint16 liceningStreamPort();
    const proto::receiver::DeviceSetInfo &getDeviceSetInfo()const;
    QString getCurrentDeviceSetName()const;

    QStringList receiverNameList()const;
    QString getStationAddress()const;
    ShPtrPacketBuffer getDDC1Buffer()const;
signals:
    void connected();
    void disconnected();

    void commandSuccessed();
    void commandFailed(const QString &errorString);
    void deviceSetReady();
    void deviceInfoUpdated();
    void ddc1StreamStarted();
    void ddc1StreamStoped();

private:
    void readAnswerPacket(const proto::receiver::Answer&answer);
    QString errorString(proto::receiver::CommandType commandType);
private slots:
    void onMessageReceived(const QByteArray &buffer);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
