#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "channel_host.h"

#include "wrd_coh_g35_ds.h"

#include <memory>
#include <QObject>

class ChannelHost;

class DeviceSetClient:public QObject
{
    Q_OBJECT

    static const int SLEEP_TIME=100;
public:
    DeviceSetClient(net::ChannelHost*channelHost);
    DeviceSetClient(net::ChannelHost*channelHost,
                    const std::shared_ptr<CohG35DeviceSet>&deviceSet);
    ~DeviceSetClient();

    void setCohDeviceSet(const std::shared_ptr<CohG35DeviceSet>&shPtrCohG35DeviceSet);
    const std::shared_ptr<CohG35DeviceSet> &getCohDeviceSet();
signals:
    void stationDisconnected();
    void changedDeviceSet(unsigned int indexDeviceSet);
private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray &buffer);
private:
    void sendDeviceSetInfo();
    void sendCommandAnswer(proto::receiver::Answer *commandAnswer);
    void readCommandPacket(const proto::receiver::Command &command);
    DeviceSetSettings extractSettingsFromCommand(const proto::receiver::Command &command);

//    QByteArray serializeMessage(const google::protobuf::Message &message);
    void writeMessage(const google::protobuf::Message &message);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
