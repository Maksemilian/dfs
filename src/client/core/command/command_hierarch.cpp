#include "command_hierarch.h"

#include "core/sync_manager.h"

#include "interface/i_device_set_settings.h"
#include "ui/db/widget_director.h"


AbstractCommand::AbstractCommand(){}

AbstractCommand::~AbstractCommand()=default;

TimerCommand::TimerCommand():AbstractCommand ()
{
    std::shared_ptr<QMetaObject::Connection> sharedPtrConnection(new QMetaObject::Connection) ;
    *sharedPtrConnection= connect(&timer,&QTimer::timeout,
                                  this,&TimerCommand::onTimerTimeout);
}
void TimerCommand::onTimerTimeout()
{
    if(done) {
        qDebug()<<"TIMER COMMAND DONE ON";
        timer.stop();
    }
}

bool TimerCommand::isDone()
{
    return done;
}

void TimerCommand::setDone(bool done)
{
    this->done=done;
}

TimerCommand::~TimerCommand(){}

ReceiverCommand::ReceiverCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :TimerCommand (),syncManager(syncManager),subject(subject)
{
}

ReceiverCommand::~ReceiverCommand()=default;

AddTaskCommand::AddTaskCommand(WidgetDirector *widgetDirector,IDeviceSetSettings*subject)
    :TimerCommand (),wd(widgetDirector),subject(subject)
{
    QObject::connect(wd,&WidgetDirector::taskAdded,[this](){
        done=true;
        this->subject->setArrowCursor();
    });
}

void AddTaskCommand::execute()
{
    subject->setWaitCursor();
    wd->onAddSelectedTask(QString::number(subject->getDDC1Frequency()),QDateTime::currentDateTime());
}

//************************* SYNC START

SyncStartCommand::SyncStartCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand (syncManager,subject)
{
    QObject::connect(syncManager,&SyncManager::syncStarted,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"SyncStartCommand::SyncStartCommand";
    });
}

void SyncStartCommand::execute()
{
    subject->setWaitCursor();
    qDebug()<<"SR:"<<subject->getSampleRateForBandwith()<<"SB:"<< subject->getSamplesPerBuffer();
    syncManager->start(subject->getDDC1Frequency(),
                subject->getSampleRateForBandwith(),
                       subject->getSamplesPerBuffer());
}

//************************* SYNC STOP

SyncStopCommand::SyncStopCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand (syncManager,subject)
{
    QObject::connect(syncManager,&SyncManager::syncStoped,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"SyncStopCommand::SyncStopCommand";
    });
}

void SyncStopCommand::execute()
{
    subject->setWaitCursor();
    syncManager->stop();
}

//************************* ATT

AttenuatorCommand::AttenuatorCommand(SyncManager*syncManager,IDeviceSetSettings*subject):
    ReceiverCommand(syncManager,subject)
{
    connect(syncManager,&SyncManager::changedAttenuator,[this]{
        done=true;
        this->subject->setArrowCursor();
        //        qDebug()<<"PowerCommandOn::PowerCommandOn/";
    });
}

void AttenuatorCommand::execute()
{
    subject->setWaitCursor();
    syncManager->setBroadcastAttenuator(subject->getAttenuator());
}

//************************* PRES

PreselectorCommand::PreselectorCommand(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)
{
    connect(syncManager,&SyncManager::changedPrelector,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOn::PowerCommandOn";
    });
}

void PreselectorCommand::execute()
{
    subject->setWaitCursor();
    syncManager->setBroadcastPreselector(subject->getPreselectors().first,
                                         subject->getPreselectors().second);
}

//************************* PREAM

PreamplifireCommand::PreamplifireCommand(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::changedPreamplifierEnabled,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOn::PowerCommandOn";
    });
}

void PreamplifireCommand::execute()
{
    subject->setWaitCursor();
    syncManager->setBroadcastPreamplifierEnabled(subject->getPreamplifierEnabled());
}

//************************* ADC EN

AdcEnabledCommand::AdcEnabledCommand(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::changedAdcEnabled,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOn::PowerCommandOn";
    });
}

void  AdcEnabledCommand::execute()
{
    subject->setWaitCursor();
    syncManager->setBroadcastAdcEnabled(subject->getAdcNoiceBlankerEnabled());
}

//************************* ADC THR

AdcThresholdCommand::AdcThresholdCommand(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::changedAdcThreshold,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOn::PowerCommandOn";
    });
}

void AdcThresholdCommand::execute()
{
    subject->setWaitCursor();
    syncManager->setBroadcastAdcThreshold(subject->getAdcNoiceBlankerThreshold());
}

//************************* POWER ON

PowerCommandOn::PowerCommandOn(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)    {

    connect(syncManager,&SyncManager::settedPowerOn,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOn::PowerCommandOn";
    });
}

void PowerCommandOn::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->setBroadcastPowerOn();
}

//************************* POWER OFF

PowerCommandOff::PowerCommandOff(SyncManager*syncManager,IDeviceSetSettings *subject)
    : ReceiverCommand(syncManager,subject)    {

    connect(syncManager,&SyncManager::settedPowerOff,[this]{
        done=true;
        this->subject->setArrowCursor();
        qDebug()<<"PowerCommandOff::PowerCommandOff";
    });
}

void PowerCommandOff::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->setBroadcastPowerOff();
}

//************************* SETTINGS

SettingsCommand::SettingsCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::settedSettings,[this]{
        done=true;
        this->subject->setArrowCursor();
    });
}

void SettingsCommand::execute()
{
    done=false;
    QVariant v;
    v.setValue(subject->getSettings());

    subject->setWaitCursor();
    syncManager->setBroadcastSettings(subject->getSettings());
}

//************************* START DDC

StartDDC1Command::StartDDC1Command(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::startedDdc1,[this]{
        done=true;
        this->subject->setArrowCursor();
    });
}

void StartDDC1Command::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->startBroadcastDDC1(subject->getSamplesPerBuffer());
}

//************************* STOP DDC

StopDDC1Command::StopDDC1Command(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::stopedDdc1,[this]{
        done=true;
        this->subject->setArrowCursor();
    });
}

void StopDDC1Command::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->stopBroadcastDDC1();
}

//************************* RESTART

SetDDC1TypeCommand::SetDDC1TypeCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::changedDDC1Type,[this]{
        done=true;
        this->subject->setArrowCursor();
    });
}

void SetDDC1TypeCommand::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->setBroadcastDDC1Type(subject->getDDC1Type());
}

//************************* FREQ

FrequencyCommand::FrequencyCommand(SyncManager*syncManager,IDeviceSetSettings*subject)
    :ReceiverCommand(syncManager,subject)    {
    connect(syncManager,&SyncManager::changedDDC1Freq,[this]{
        done=true;
        this->subject->setArrowCursor();
    });
}

void FrequencyCommand::execute()
{
    done=false;
    subject->setWaitCursor();
    syncManager->setBroadcastDDC1Frequency(subject->getDDC1Frequency());
}

//************************* MACRO

MacroCommand::MacroCommand(){}

void MacroCommand::onTimerTimeout()
{
    if(commands[commandCounter]->isDone()){
        commandCounter++;

        if(commandCounter<commands.size()){
            waitTime.start();
            commands[commandCounter]->execute();
            qDebug()<<"*************MACRO EXEC";
        }else {
            qDebug()<<"*********************STOP MACRO";
            stop();
        }
    }else if (waitTime.elapsed()>WAIT_TIME_MS) {
        qDebug()<<"*********************ERROR MACRO COMMAND";
        stop();
    }
}

void MacroCommand::stop()
{
    commandCounter=0;
    timer.stop();
}

void MacroCommand::execute()
{
    qDebug()<<"====================MackroCommand EXEC";
    timer.start(TimerCommand::TIMER_TIMEOUT);
    waitTime.start();
    commands[commandCounter]->execute();
}

void MacroCommand::addCommand(TimerCommand*command)
{
    commands<<command;
}

void MacroCommand::removeCommand(TimerCommand*command)
{
    commands.removeOne(command);
}

