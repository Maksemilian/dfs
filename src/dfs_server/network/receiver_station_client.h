#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "channel_host.h"

#include "receiver.pb.h"

#include <google/protobuf/message.h>
#include <memory>
#include <QObject>

class CohG35DeviceSetSettings;
class ChannelHost;

class DeviceSetClient:public QObject
{
    Q_OBJECT

    static const int WAITING_SETTING_DEVICE_SET_POWER=100;
    static const int SLEEP_TIME=100;
public:
    DeviceSetClient(net::ChannelHost*channelHost);
    ~DeviceSetClient();

    Q_SIGNAL void stationDisconnected();
    void sendDeviceSetInfo();

private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray &buffer);
private:
    void sendCommandAnswer(proto::receiver::Answer *commandAnswer);

    void readCommanPacket(const proto::receiver::Command &command);
    CohG35DeviceSetSettings extractSettingsFromCommand(const proto::receiver::Command &command);

    QByteArray serializeMessage(const google::protobuf::Message &message);
    void writeMessage(const google::protobuf::Message &message);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
