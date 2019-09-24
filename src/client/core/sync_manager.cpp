#include "sync_manager.h"
#include "core/network/receiver_station_client.h"
#include "command.pb.h"

#include <QDebug>

#include <core/network/receiver_station_client.h>

const quint16 SyncManager::TIME_WAIT_RESPONCE=10000;
const quint16 SyncManager::TIME_SINGLE_SHOT=200;

SyncManager::SyncManager()
{
    listen(QHostAddress::Any,LISTEN_PORT);
}

int SyncManager::countStation()
{
    return listeners.size();
}

void SyncManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"Receiver Client connected:"<<socketDescriptor;
    auto deleter=[](ReceiverStationClient *client){
        client->deleteLater();
    };

    SptrReceiverStationClient stationClient(new ReceiverStationClient(socketDescriptor),deleter);

    //    listeners<<stationClient;
    listeners.insert(socketDescriptor,stationClient);

    Q_ASSERT_X(stationClient,"MainWindow::onStaionReadyForUse","station id null");

    connect(stationClient.get(),&ReceiverStationClient::connected,
            this,&SyncManager::onStationReady);

    connect(stationClient.get(),&ReceiverStationClient::disconnected,
            this,&SyncManager::onStationDisconnected);
}

void SyncManager::onStationReady()
{
    ReceiverStationClient *connectedStation=qobject_cast<ReceiverStationClient*>(sender());
    if(connectedStation){
        auto ptr =  std::find_if(begin(listeners), end(listeners),
                                 [&](SptrReceiverStationClient const& current)
        {
                return current.get() == connectedStation;
    });

        QStringList receiveresNames=(*ptr)->getCurrentDeviceSetReceiversNames();

        if(countStation()>=SyncManager::MIN_NUMBER_STATIONS_FOR_SYNC){
            emit syncReady();
            qDebug()<<"Sync Ready";
        }

        emit stationConnected(ptr.key(),
                              (*ptr)->getStationAddress(),
                              QString::number(SERVER_STREAM_PORT),
                              receiveresNames);

        qDebug()<<"********************SyncManager station ready"
               <<ptr.key()<<ptr->use_count()<<countStation();
    }
}

void SyncManager::onStationDisconnected()
{
    ReceiverStationClient *disconnectedStation=qobject_cast<ReceiverStationClient*>(sender());
    if(disconnectedStation){
        auto ptr =
                std::find_if(begin(listeners), end(listeners),
                             [&](SptrReceiverStationClient const& current)
        {
                return current.get() == disconnectedStation;
    });
        qDebug()<<"********************SyncManager station disconnected"
               <<ptr.key()<<ptr->use_count()<<countStation();

        emit stationDisconnected(ptr.key());
        listeners.erase(ptr);

        if(countStation()<SyncManager::MIN_NUMBER_STATIONS_FOR_SYNC){
            emit syncNotReady();
            qDebug()<<"Sync Not Ready";
        }
    }
}

void SyncManager::addSyncSignalUpdater(ISyncSignalUpdate *updater)
{
    sync.addSyncSignalUpdater(updater);
}

void SyncManager::addSumDivUpdater(ISumDivSignalUpdate *updater)
{
    sync.addSymDivUpdater(updater);
}

//***************** SET BROADCAST COMMAND *******************

void SyncManager::setBroadcastDDC1Frequency(quint32 ddc1Frequency)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_DDC1_FREQUENCY);

    for(const SptrReceiverStationClient&client:listeners)
        client->setDDC1Frequency(ddc1Frequency);
}

void SyncManager::setBroadcastDDC1Type(quint32 ddc1Type)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_DDC1_TYPE);

    for(const SptrReceiverStationClient&client:listeners)
        client->setDDC1Type(ddc1Type);
}

void SyncManager::setBroadcastAttenuator(quint32 attenuationLevel)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_ATTENUATOR);

    for(SptrReceiverStationClient client:listeners)
        client->setAttenuator(attenuationLevel);
}

void SyncManager::setBroadcastPreselector(quint32 lowFrequency, quint32 highFrequency)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_PRESELECTORS);

    for(const SptrReceiverStationClient&client:listeners)
        client->setPreselectors(lowFrequency,highFrequency);
}

void SyncManager::setBroadcastPreamplifierEnabled(bool state)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_PREAMPLIFIER_ENABLED);

    for(const SptrReceiverStationClient&client:listeners)
        client->setPreamplifierEnabled(state);
}

void SyncManager::setBroadcastAdcEnabled(bool state)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_ADC_NOICE_BLANKER_ENABLED);

    for(const SptrReceiverStationClient&client:listeners)
        client->setAdcNoiceBlankerEnabled(state);
}

void SyncManager::setBroadcastAdcThreshold(quint16 threshold)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD);

    for(const SptrReceiverStationClient&client:listeners)
        client->setAdcNoiceBlankerThreshold(threshold);
}

void SyncManager::setBroadcastPowerOn()
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_POWER_ON);

    for(const SptrReceiverStationClient&client:listeners)
        client->setPower(true);
}

void SyncManager::setBroadcastPowerOff()
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_POWER_OFF);

    for(const SptrReceiverStationClient&client:listeners)
        client->setPower(false);
}

void SyncManager::setBroadcastSettings(const DeviceSetSettings &deviceSetSettings)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::SET_SETTINGS);

    for(const SptrReceiverStationClient&client:listeners)
        client->setSettings(deviceSetSettings);
}

void SyncManager::startBroadcastDDC1(quint32 samplePerBuffer)
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::START_DDC1);

    for(const SptrReceiverStationClient&client:listeners)
        client->startDDC1Stream(samplePerBuffer);
}

void SyncManager::stopBroadcastDDC1()
{
    checkTimer.start();
    checkReceiverStationCommand(CommandType::STOP_DDC1);

    for(const SptrReceiverStationClient&client:listeners)
        client->stopDDC1Stream();
}

void SyncManager::start(quint32 ddcFrequency,quint32 sampleRate, quint32 blockSize)
{
    //    qDebug()<<"ET_SYNC"<<"SR:"<<sampleRate<<"BS:"<<blockSize;

    StreamReadablePair streamReadablePair;
    //TODO ВОЗНИКАЕТ ОШИБКА ЕСЛИ РАСКОМИТИТЬ
//    streamReadablePair.first=listeners.first();
//    streamReadablePair.second=listeners.last();
    sync.start(streamReadablePair,ddcFrequency,sampleRate,blockSize);

    emit syncStarted();
}

void SyncManager::stop()
{
    sync.stop();
    emit syncStoped();
}

void SyncManager::checkReceiverStationCommand(quint32 type)
{
    //    qDebug()<<"checkReceiverStationCommand";
    QTimer::singleShot(TIME_SINGLE_SHOT,
                       [this,type]{
        if(waitCommandState(type)){
            sendSuccessSignal(type);
        }else if(checkTimer.elapsed()<TIME_WAIT_RESPONCE){
            checkReceiverStationCommand(type);
        }else{ sendErrorString(type); }
    } );
}

bool SyncManager::waitCommandState(quint32 type)
{
    bool stateSuccessPower=true;

    for(const SptrReceiverStationClient&station:listeners)
        if(!station->checkStateCommand(type)){
            stateSuccessPower=false;
            break;
        }

    return stateSuccessPower;
}
//TODO ДОБАВИТЬ СОЗДАНИЕ ДДС ПОТОКОВ
/*
//******************* START DDC STREAM ***********************

connect(this,&ReceiverStationClient::deviceSetChangeBandwith,[this]{
        d->streamReader->resetBuffer();
    });

    connect(this,&ReceiverStationClient::ddc1StreamStarted,
            this,&ReceiverStationClient::startDDC1StreamReader);

    connect(this,&ReceiverStationClient::ddc1StreamStoped,
            this,&ReceiverStationClient::stopDDC1StreamReader);


bool ReceiverStationClient::readDDC1StreamData(Packet &data)
{
    return   d->streamReader->readDDC1StreamData(data);
}

void ReceiverStationClient::startDDC1StreamReader()
{
    if(d->streamReader.get()==nullptr){
        //        d->streamReader.reset(new SignalStreamReader(BUFFER_SIZE,getPeerAddress(),
        //                                                     static_cast<unsigned short>(getDeviceSetInfo().signal_port())));
        d->streamReader.reset(new StreamReader(getPeerAddress(),
                                               8000,//TODO ЗАМЕНИТ НА КОНСТАНТУ
                                               std::make_shared<RingPacketBuffer>(BUFFER_SIZE)));

    }
    //    d->streamReader->startDDC1Stream();
    d->streamReader->start();
}

void ReceiverStationClient::stopDDC1StreamReader()
{
    if(d->streamReader.get()!=nullptr){
        //        d->streamReader->stopDDC1Stream();
        //        d->streamReader->bufferReset();
        d->streamReader->stop();
        d->streamReader->resetBuffer();
    }
}
*/

void SyncManager::sendSuccessSignal(quint32 commandType)
{
    switch (commandType) {
    case CommandType::SET_POWER_ON:
        qDebug()<<"POWER CHECK ON";
        emit settedPowerOn();
        break;
    case CommandType::SET_POWER_OFF:
        qDebug()<<"POWER CHECK OFF";
        emit settedPowerOff();
        break;
    case CommandType::SET_SETTINGS:
        qDebug()<<"SETTINGS CHECk";
        emit settedSettings();
        break;
    case CommandType::START_DDC1:
        qDebug()<<"Start ddc1 check";
        emit startedDdc1();
        break;
    case CommandType::STOP_DDC1:
        qDebug()<<"Stop ddc1 check";
        emit stopedDdc1();
        break;
    case CommandType::SET_DDC1_TYPE:
        qDebug()<<"Restart ddc1 check";
        emit changedDDC1Type();
        break;
    case CommandType::SET_DDC1_FREQUENCY:
        qDebug()<<"Change Freq";
        emit changedDDC1Freq();
        break;
    case CommandType::SET_PRESELECTORS:
        qDebug()<<"Presselector check";
        emit changedPrelector();
        break;
    case CommandType::SET_PREAMPLIFIER_ENABLED:
        qDebug()<<"Pream check";
        emit changedPreamplifierEnabled();
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
        qDebug()<<"ADC ENABLED CHECK";
        emit changedAdcEnabled();
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
        qDebug()<<"ADC THRESHOLD CHECK";
        emit changedAdcThreshold();
        break;
    case CommandType::SET_ATTENUATOR:
        qDebug()<<"ATTENUATOR CHECK";
        emit changedAttenuator();
        break;
    }
}

void SyncManager::sendErrorString(quint32 commandType)
{
    switch (commandType) {
    case CommandType::SET_POWER_ON:
        emit commandError("ERROR Command Power on");
        break;
    case CommandType::SET_POWER_OFF:
        emit commandError("ERROR Command Power off ");
        break;
    case CommandType::SET_SETTINGS:
        emit commandError("ERROR Command Power set settings");
        break;
    case CommandType::START_DDC1:
        emit commandError("ERROR Command Power start ddc1");
        break;
    case CommandType::STOP_DDC1:
        emit commandError("ERROR Command Power stop ddc1");
        break;
    case CommandType::SET_DDC1_TYPE:
        emit commandError("ERROR Command Power set ddc1 type");
        break;
    case CommandType::SET_DDC1_FREQUENCY:
        commandError("ERROR Change Freq");
        emit changedDDC1Freq();
        break;
    case CommandType::SET_PRESELECTORS:
        commandError("ERROR Presselector check");
        emit changedPrelector();
        break;
    case CommandType::SET_PREAMPLIFIER_ENABLED:
        commandError("ERROR Pream check");
        emit changedPreamplifierEnabled();
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_ENABLED:
        commandError("ERROR ADC ENABLED CHECK");
        emit changedAdcEnabled();
        break;
    case CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD:
        commandError("ERROR ADC THRESHOLD CHECK");
        emit changedAdcThreshold();
        break;
    case CommandType::SET_ATTENUATOR:
        commandError("ERROR ATTENUATOR CHECK");
        break;
    }
}

//********************** LOAD TASK *********************************

void SyncManager::loadBroacastFile(const QString &fileName, const QDate &fileDate)
{
    Task *task=new Task(fileName,listeners.size());//WARNING задача удаляется по сигналу finished
    connect(task,&Task::finished,task,&Task::deleteLater);

    for (const SptrReceiverStationClient&station:listeners) {

        QString filePath=station->getCurrentDeviceSetName()+
                "/"+fileDate.toString("yyyy-MM-dd")+
                "/"+fileName;
        qDebug()<<"SyncManager::loadTask"<<filePath;
        station->startLoadingFiles(QStringList()<<filePath);

        connect(station.get(),&ReceiverStationClient::receivedFileSize,
                task,&Task::onReceiveFileSize);

        connect(task,&Task::receivedTaskSize,this,&SyncManager::taskSize);
    }
}
