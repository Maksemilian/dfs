#ifndef COMMAND_HIERARCH_H
#define COMMAND_HIERARCH_H

#include <QObjectUserData>

#include <QTimer>
#include <QTime>
#include <QDebug>

class IDeviceSetSettings;
class IDeviceSet;
class SyncManager;
class WidgetDirector;

class AbstractCommand:public QObjectUserData
{
public:
      AbstractCommand();
      virtual ~AbstractCommand();
      virtual void execute()=0;
};


class TimerCommand:public QObject,public AbstractCommand
{
    Q_OBJECT
public:
    static const int TIMER_TIMEOUT=200;
    TimerCommand();
    virtual ~TimerCommand();
    virtual void onTimerTimeout();
    bool isDone();
    void setDone(bool done);
protected:
    QTimer timer;
    bool done;
};

class ReceiverCommand:public AbstractCommand
{
public:
     ReceiverCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
     ~ReceiverCommand();
protected:
    SyncManager*syncManager;
    IDeviceSet*_iDeviceSet;
    IDeviceSetSettings*subject;

};

//class AddTaskCommand:public TimerCommand
//{
//public:
//    AddTaskCommand(WidgetDirector*widgetDirector,IDeviceSetSettings*subkect);
//    void execute()override;
//private:
//    WidgetDirector*wd;
//    IDeviceSetSettings*subject;
//};

//class SyncStartCommand:public ReceiverCommand
//{
//public:
//    SyncStartCommand(SyncManager*syncManager,IDeviceSetSettings*subject);
//    void execute()override;
//};


//class SyncStopCommand:public ReceiverCommand
//{
//public:
//    SyncStopCommand(SyncManager*syncManager,IDeviceSetSettings*subject);
//    void execute()override;
//};
//************************* ATTENUATOR
class AttenuatorCommand:public ReceiverCommand
{
public:
    AttenuatorCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    AttenuatorCommand();
    void execute()override;
};

//************************* PRESELECTORS
class PreselectorCommand:public ReceiverCommand
{
public:
    PreselectorCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* PREAM
class PreamplifireCommand:public ReceiverCommand
{
public:
    PreamplifireCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* ADC ENABLED
class AdcEnabledCommand:public ReceiverCommand
{
public:
    AdcEnabledCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* ADC THRESHOLD
class AdcThresholdCommand:public ReceiverCommand
{
public:
    AdcThresholdCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* POWER ON
class PowerCommandOn:public ReceiverCommand
{
public:
    PowerCommandOn(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* POWER OFF
class PowerCommandOff:public ReceiverCommand
{
public:
    PowerCommandOff(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* SETTINGS
class SettingsCommand:public ReceiverCommand
{
public:
    SettingsCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute()override;
};

//************************* START DDC1
class StartDDC1Command:public ReceiverCommand
{
public:
    StartDDC1Command(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute() override;
};

//************************* STOP DDC1
class StopDDC1Command:public ReceiverCommand
{
public:
    StopDDC1Command(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute() override;
};

//************************* RESTART
class SetDDC1TypeCommand:public ReceiverCommand
{
public:
    SetDDC1TypeCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute() override;
};

//************************* FREQ
class FrequencyCommand:public ReceiverCommand
{
public:
    FrequencyCommand(IDeviceSet*iDeviceSet,IDeviceSetSettings*subject);
    void execute() override;
};
//************************* MACRO
class MacroCommand:public AbstractCommand
{
//    static const int WAIT_TIME_MS=3000;
public:
    MacroCommand();
//    void onTimerTimeout()override;
    void execute()override;
    void addCommand(ReceiverCommand*command);
    void removeCommand(ReceiverCommand*command);
private:
    QList<ReceiverCommand*>_commands;
//    void stop();
//    int commandCounter=0;
//    QList<TimerCommand*>commands;
//    QTime waitTime;
};

#endif // COMMAND_HIERARCH_H
