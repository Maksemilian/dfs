#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include <google/protobuf/message.h>
#include <memory>

#include <QObject>
#include "receiver.pb.h"

class CohG35DeviceSet;
class Command;
class PeerWireClient;
class CohG35DeviceSetSettings;
class QHostAddress;

class ReceiverStationClient:public QObject
{
    Q_OBJECT

    static const int WAITING_SETTING_DEVICE_SET_POWER=100;
    static const int SLEEP_TIME=100;
public:
    ReceiverStationClient(quint16 signalPort,quint16 filePort);
    ~ReceiverStationClient();

    bool connectToHost(const QHostAddress &address,quint16 port);
    bool setupNewDeviceSet(quint32 deviceSetIndex);

    Q_SIGNAL void stationDisconnected();
private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray &buffer);
private:
    void sendDeviceSetInfo(quint16 signalPort=8000,quint16 filePort=7000);
    void sendStateWorkingCommand(quint32 type,bool state);

    void readCommanPacket(const proto::receiver::Command &command);
    CohG35DeviceSetSettings extractSettingsFromCommand(const proto::receiver::Command &command);

    QByteArray serializeMessage(const google::protobuf::Message &message);
    void writeMessage(const google::protobuf::Message &message);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
