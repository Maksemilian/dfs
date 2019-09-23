#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "interface/i_stream_reader.h"
#include "receiver.pb.h"
struct DeviceSetSettings;

class ReceiverStationClient;
class RingPacketBuffer;

class Packet;

class Command;
class Answer;
class DeviceSetInfo;

class QHostAddress;
class QHostAddress;
class QByteArray;

class StreamReader:IStreamRead
{
public:
    StreamReader(const QHostAddress &address,quint16 port,
                 const std::shared_ptr<RingPacketBuffer>&streamBuffer);
    bool readDDC1StreamData(Packet&data) override;
    void start();
    void stop();
    void resetBuffer();
private:
    void run();
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};

class ReceiverStationClient: public QObject,public IStreamRead
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
    ReceiverStationClient(qintptr socketDescriptor,QObject *parent=nullptr);
    ~ReceiverStationClient()override;

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

    bool readDDC1StreamData(Packet &data)override;
    //WARNING СЮДА ПЕРЕДАЕТСЯ ТИП КОМАНДЫ
    bool checkStateCommand(quint32 type);

    void startLoadingFiles(const QStringList &fileNames);
    void stopLoadingFiles();

    const proto::receiver::DeviceSetInfo &getDeviceSetInfo()const;
    QString getCurrentDeviceSetName();
    QStringList getCurrentDeviceSetReceiversNames();
    QHostAddress getPeerAddress();
    QString getStationAddress();

Q_SIGNALS:
    void connected();
    void disconnected();

    void deviceSetReadyForUse();

    void deviceSetPowerSetted(bool state);
    void deviceSetSettingsSetted();
    void deviceSetChangeBandwith();

    void ddc1StreamStarted();
    void ddc1StreamStoped();

    Q_SIGNAL void receivedFileSize(int fileIndex,qint64 fullSize);
    Q_SIGNAL void bytesProgressFile(int fileIndex, qint64 bytesReaded,qint64 bitesSize);
private:
//    const QByteArray serializeCommandToByteArray(const Command &command);
    const QByteArray serializeCommandToByteArray(const proto::receiver::Command &command);
    void writeToConnection(const QByteArray &commandData);

//    void sheduleTransfer();
    void readAnswerPacket(const proto::receiver::Answer&answer);
//WARNING СЮДА ПЕРЕДАЕТСЯ ТИП КОМАНДЫ
    void setCurrentCommandType(quint32 type);

    void startDDC1StreamReader();
    void stopDDC1StreamReader();

private Q_SLOTS:
//    Q_SLOT void transfer();
//    Q_SLOT void onReadyReadInfo();
//    Q_SLOT void onReadyReadAnswer();
    Q_SLOT void onMessageReceived(const QByteArray &buffer);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // RECEIVER_STATION_CLIENT_H
