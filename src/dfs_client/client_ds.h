#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "receiver.pb.h"
#include <QObject>

//struct DeviceSetSettings;

class QHostAddress;
class DeviceSetClient: public QObject
{
    Q_OBJECT
public:
    DeviceSetClient(QObject *parent=nullptr);
    ~DeviceSetClient();

    void connectToHost(const QHostAddress &address,quint16 port);
    void disconnectFromHost();
    void sendCommand(const proto::receiver::Command &command);

    const proto::receiver::DeviceSetInfo &getDeviceSetInfo()const;
    QString getCurrentDeviceSetName();

    QStringList receiverNameList();
    QString getStationAddress();
signals:
    void connected();
    void disconnected();

    void commandSuccessed();
    void commandFailed(const QString &errorString);
    void deviceSetReady();

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
