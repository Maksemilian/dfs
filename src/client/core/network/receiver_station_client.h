#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "interface/i_stream_reader.h"
#include "receiver.pb.h"

struct DeviceSetSettings;

class ReceiverStationClient;


class QHostAddress;
class QByteArray;

class ReceiverStationClient: public QObject
{
    Q_OBJECT
    enum ReceiverStationError{
        RSE_ATTENUATOR,
        RSE_POWER,
        RSE_SETTINGS,
        RSE_START_DDC1,
        RSE_STOP_DDC1
    };
    static const int BUFFER_SIZE=4;
public:
    ReceiverStationClient(QObject *parent=nullptr);
    ~ReceiverStationClient();

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

    //WARNING СЮДА ПЕРЕДАЕТСЯ ТИП КОМАНДЫ
    bool checkStateCommand(quint32 type);

    void startLoadingFiles(const QStringList &fileNames);
    void stopLoadingFiles();

    const proto::receiver::DeviceSetInfo &getDeviceSetInfo()const;
    QString getCurrentDeviceSetName();
    QStringList getCurrentDeviceSetReceiversNames();
    QString getStationAddress();
    void connectToHost(const QHostAddress &address,quint16 port);
signals:
    void connected();
    void disconnected();

    void deviceSetReadyForUse();

    void deviceSetPowerSetted(bool state);
    void deviceSetSettingsSetted();
    void deviceSetChangeBandwith();

    void ddc1StreamStarted();
    void ddc1StreamStoped();

    void receivedFileSize(int fileIndex,qint64 fullSize);
    void bytesProgressFile(int fileIndex, qint64 bytesReaded,qint64 bitesSize);
private:
    void sendCommand(proto::receiver::Command &command);
    const QByteArray serializeCommandToByteArray(const google::protobuf::Message &command);
    void readAnswerPacket(const proto::receiver::Answer&answer);
//WARNING СЮДА ПЕРЕДАЕТСЯ ТИП КОМАНДЫ
    void setCurrentCommandType(quint32 type);
private slots:
    void onMessageReceived(const QByteArray &buffer);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
