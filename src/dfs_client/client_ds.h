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
/*
    void setSettings(const DeviceSetSettings &settings);

    void setPower(bool state);
    void setAttenuator(quint32 attenuator);
    void setPreselectors(quint32 lowFrequency,quint32 highFrequency);
    void setAdcNoiceBlankerEnabled(bool state);
    void setAdcNoiceBlankerThreshold(quint16 threshold);
    void setPreamplifierEnabled(bool state);
    void setDDC1Frequency(quint32 ddc1Frequency);

    void setDDC1Type(quint32 typeInde);

    void startDDC1Stream(quint32 samplesPerBuffer);
    void stopDDC1Stream();
*/
    const proto::receiver::DeviceSetInfo &getDeviceSetInfo()const;
    QString getCurrentDeviceSetName();

    QStringList receiverNameList();
    QString getStationAddress();
    void connectToHost(const QHostAddress &address,quint16 port);
    void disconnectFromHost();
    void sendCommand(const proto::receiver::Command &command);
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
    //TODO ТАКАЯ ЖЕ КОМАНДА ЕСТЬ НА СТОРОНЕ СЕРВЕРА
    const QByteArray serializeCommandToByteArray(const google::protobuf::Message &command);
    QString errorString(proto::receiver::CommandType commandType);
private slots:
    void onMessageReceived(const QByteArray &buffer);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
