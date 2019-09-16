#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include <google/protobuf/message.h>
#include <memory>

#include <QObject>

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
private:
    Q_SLOT void onReadyRead();
    Q_SLOT void onDisconnected();

private:
    void sendDeviceSetInfo(quint16 signalPort=8000,quint16 filePort=7000);
    void sendStateWorkingCommand(quint32 type,bool state);

    void parseMessage(const QByteArray &baCommand);
    CohG35DeviceSetSettings extractSettingsFromCommand(const Command &command);

    QByteArray serializeMessage(const google::protobuf::Message &message);
    void writeMessage(const google::protobuf::Message &message);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
