#include "sync_manager.h"
#include "core/network/receiver_station_client.h"
#include "command.pb.h"

#include <QDebug>

#include <core/network/receiver_station_client.h>

const quint16 SyncManager::TIME_WAIT_RESPONCE=10000;
const quint16 SyncManager::TIME_SINGLE_SHOT=200;

SyncManager::SyncManager()
{
}

void SyncManager::connectToStation(const QHostAddress &address, quint16 port)
{
    auto deleter=[](ReceiverStationClient *client){
        client->deleteLater();
    };
    SptrReceiverStationClient stationClient(new ReceiverStationClient(),deleter);
    connect(stationClient.get(),&ReceiverStationClient::commandSuccessed,
            this,&SyncManager::onCommandSuccesed);
    //    listeners<<stationClient;
    listeners.insert(address.toIPv4Address(),stationClient);
    //    listenersT.insert(address.toString(),stationClient);
    Q_ASSERT_X(stationClient,"MainWindow::onStaionReadyForUse","station id null");

    connect(stationClient.get(),&ReceiverStationClient::connected,
            this,&SyncManager::onStationReady);

    connect(stationClient.get(),&ReceiverStationClient::disconnected,
            this,&SyncManager::onStationDisconnected);

    connect(stationClient.get(),&ReceiverStationClient::commandFailed,
            this,&SyncManager::onCommandFailed);

    stationClient->connectToHost(address,port);
}

int SyncManager::countStation()
{
    return listeners.size();
}


void SyncManager::onStationReady()
{
    qDebug()<<"On Station Ready";
    ReceiverStationClient *connectedStation=qobject_cast<ReceiverStationClient*>(sender());
    if(connectedStation){
        auto ptr =  std::find_if(begin(listeners), end(listeners),
                                 [&](SptrReceiverStationClient const& current)
        {
                return current.get() == connectedStation;
    });

        QStringList receiveresNames=(*ptr)->getCurrentDeviceSetReceiversNames();

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
    }
}

void SyncManager::onCommandFailed(const QString &error)
{
    ReceiverStationClient*client=qobject_cast<ReceiverStationClient*>(sender());
    if(client)
        qDebug()<<client->getStationAddress()<<error;
}

void SyncManager::onCommandSuccesed()
{
    qDebug()<<"onCommandSucessed";
    if(!commandQueue.isEmpty())
        commandQueue.dequeue();


    if(commandQueue.isEmpty()){
        qDebug()<<"Command Queue is empty";
        emit commandSucessed();
        return;
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


void SyncManager::start(quint32 ddcFrequency,quint32 sampleRate, quint32 blockSize)
{
    //    qDebug()<<"ET_SYNC"<<"SR:"<<sampleRate<<"BS:"<<blockSize;

//    StreamReadablePair streamReadablePair;
    //TODO ВОЗНИКАЕТ ОШИБКА ЕСЛИ РАСКОМИТИТЬ
    //    streamReadablePair.first=listeners.first();
    //    streamReadablePair.second=listeners.last();
//    sync.start(streamReadablePair,ddcFrequency,sampleRate,blockSize);

}

void SyncManager::stop()
{
    sync.stop();
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
//        station->startLoadingFiles(QStringList()<<filePath);

//        connect(station.get(),&ReceiverStationClient::receivedFileSize,
//                task,&Task::onReceiveFileSize);

        connect(task,&Task::receivedTaskSize,this,&SyncManager::taskSize);
    }
}

//***************** SET BROADCAST COMMAND *******************

void SyncManager::setBroadcastDDC1Frequency(quint32 ddc1Frequency)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setDDC1Frequency(ddc1Frequency);
        commandQueue.enqueue(CommandType::SET_DDC1_FREQUENCY);
    }
}

void SyncManager::setBroadcastDDC1Type(quint32 ddc1Type)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setDDC1Type(ddc1Type);
        commandQueue.enqueue(CommandType::SET_DDC1_TYPE);
    }
}

//*********************
void SyncManager::setBroadcastAttenuator(quint32 attenuationLevel)
{
    for(SptrReceiverStationClient client:listeners){
        client->setAttenuator(attenuationLevel);
        commandQueue.enqueue(CommandType::SET_ATTENUATOR);
    }
}

void SyncManager::setBroadcastPreselector(quint32 lowFrequency, quint32 highFrequency)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setPreselectors(lowFrequency,highFrequency);
        commandQueue.enqueue(CommandType::SET_PRESELECTORS);
    }
}

void SyncManager::setBroadcastPreamplifierEnabled(bool state)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setPreamplifierEnabled(state);
        commandQueue.enqueue(CommandType::SET_PREAMPLIFIER_ENABLED);
    }
}

//**************************
void SyncManager::setBroadcastAdcEnabled(bool state)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setAdcNoiceBlankerEnabled(state);
        commandQueue.enqueue(CommandType::SET_ADC_NOICE_BLANKER_ENABLED);
    }
}

void SyncManager::setBroadcastAdcThreshold(quint16 threshold)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setAdcNoiceBlankerThreshold(threshold);
        commandQueue.enqueue(CommandType::SET_ADC_NOICE_BLANKER_THRESHOLD);
    }
}

void SyncManager::setBroadcastPowerOn()
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setPower(true);
        commandQueue.enqueue(CommandType::SET_POWER_ON);
    }
}

void SyncManager::setBroadcastPowerOff()
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setPower(false);
        commandQueue.enqueue(CommandType::SET_POWER_OFF);
    }
}

void SyncManager::setBroadcastSettings(const DeviceSetSettings &deviceSetSettings)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->setSettings(deviceSetSettings);
        commandQueue.enqueue(CommandType::SET_SETTINGS);
    }
}

void SyncManager::startBroadcastDDC1(quint32 samplePerBuffer)
{
    for(const SptrReceiverStationClient&client:listeners){
        client->startDDC1Stream(samplePerBuffer);
        commandQueue.enqueue(CommandType::START_DDC1);
    }
}

void SyncManager::stopBroadcastDDC1()
{
    for(const SptrReceiverStationClient&client:listeners){
        client->stopDDC1Stream();
        commandQueue.enqueue(CommandType::STOP_DDC1);
    }
}

/*
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
*/
//void SyncManager::incomingConnection(qintptr socketDescriptor)
//{
//    //    qDebug()<<"Receiver Client connected:"<<socketDescriptor;
//    //    auto deleter=[](ReceiverStationClient *client){
//    //        client->deleteLater();
//    //    };

//    //    SptrReceiverStationClient stationClient(new ReceiverStationClient(socketDescriptor),deleter);

//    //    //    listeners<<stationClient;
//    //    listeners.insert(socketDescriptor,stationClient);

//    //    Q_ASSERT_X(stationClient,"MainWindow::onStaionReadyForUse","station id null");

//    //    connect(stationClient.get(),&ReceiverStationClient::connected,
//    //            this,&SyncManager::onStationReady);

//    //    connect(stationClient.get(),&ReceiverStationClient::disconnected,
//    //            this,&SyncManager::onStationDisconnected);
//}

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

