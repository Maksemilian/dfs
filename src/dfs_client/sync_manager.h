#ifndef SYNC_MANAGER_H
#define SYNC_MANAGER_H

#include "i_device_set_settings.h"

#include "sync_pair_channel.h"

#include <QTcpServer>

#include <QTimer>
#include <QTime>
#include <QQueue>
using PairUInt32=QPair<quint32,quint32>;

Q_DECLARE_METATYPE(PairUInt32);

class ISyncSignalUpdate;
class ISumDivSignalUpdate;

class DeviceSetClient;
using SptrReceiverStationClient=std::shared_ptr<DeviceSetClient>;


class Task:public QObject
{
    Q_OBJECT
    static const int stepInPercent=5;
public:
    Task(const QString &taskName,int countStation):
        taskName(taskName),countStation(countStation){}
    Q_SIGNAL void receivedTaskSize(const QString &taskName,qint64 taskSize);
    Q_SIGNAL void finished();

    Q_SLOT void onReceiveFileSize(int fileIndex, qint64 fileSize)
    {
        Q_UNUSED(fileIndex);
        taskSize+=fileSize;

        if(++stationCounter==countStation){
            qDebug()<<"Full task size"<<taskSize<<countStation;
            emit receivedTaskSize(taskName,taskSize);
            emit finished();
        }
    }

private:
    QString taskName;
    qint64 taskSize=0;
    int countStation=0;
    int stationCounter=0;
};

class SyncManager:public QObject
{
    Q_OBJECT

    static const int TIME_FIRST=0;
    static const int TIME_SECOND=4000;
    //    static const int TIME_SECOND=0;

    static const quint16 TIME_WAIT_RESPONCE;
    static const quint16 TIME_SINGLE_SHOT;

    static const int CHANNEL_COUNT=2;
    static const quint16 LISTEN_PORT=9000;
    static const int SERVER_STREAM_PORT=9000;
    static const quint16 INCREASE_MAIN_WINDOW_WIDTH=200;
    static const quint16 INCREASE_MAIN_WINDOW_HEIGH=100;
    static const int MIN_NUMBER_STATIONS_FOR_SYNC=1;

private:

    SyncManager();
public:
    static SyncManager &instance(){
        static SyncManager inst;
        return inst;
    }
    int countStation();
    void connectToStation(const QHostAddress &address,quint16 port);

    void setBroadcastDDC1Frequency(quint32 ddc1Frequency);
    void setBroadcastAttenuator(quint32 attenuationLevel);
    void setBroadcastPreselector(quint32 lowFrequency,quint32 highFrequency);
    void setBroadcastAdcEnabled(bool state);
    void setBroadcastAdcThreshold(quint16);
    void setBroadcastPreamplifierEnabled(bool state);
    void setBroadcastDDC1Type(quint32 ddc1Type);
    void setBroadcastPowerOn();
    void setBroadcastPowerOff();
    void setBroadcastSettings(const DeviceSetSettings&deviceSetSettings);

    void startBroadcastDDC1(quint32 samplePerBuffer);
    void stopBroadcastDDC1();

    //УБРАТЬ НИЖНИЕ МЕТОДЫ
    void start(quint32 ddcFrequency,quint32 sampleRate,quint32 blockSize);
    void stop();
    void loadBroacastFile(const QString &taskName,const QDate &taskDate);
    void addSyncSignalUpdater(ISyncSignalUpdate *updater);
    void addSumDivUpdater(ISumDivSignalUpdate *updater);
signals:
    void taskSize(const QString &taskName,qint64 taskSize);
    void stationConnected(qintptr id,const QString &addr,const QString &port,
                          const QStringList &deviceSetNameList);
    void stationDisconnected(qintptr id);

    void commandError(const QString &commandError);
    void commandSucessed();
private slots:
    void onStationReady();
    void onStationDisconnected();
    void onCommandFailed(const QString &error);
    void onCommandSuccesed();
private:
    QMap<quint32,SptrReceiverStationClient>listeners;
    SyncPairChannel sync;
    QQueue<int>commandQueue;
};

#endif // SYNC_MANAGER_H
