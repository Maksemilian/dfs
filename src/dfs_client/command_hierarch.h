#ifndef COMMAND_HIERARCH_H
#define COMMAND_HIERARCH_H

#include <QObjectUserData>

#include <QDebug>

class IDeviceSettings;
class IDeviceSet;

class AbstractCommand:public QObjectUserData
{
public:
      AbstractCommand();
      virtual ~AbstractCommand();
      virtual void execute()=0;
};

class ReceiverCommand:public AbstractCommand
{
public:
     ReceiverCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
     ~ReceiverCommand();
protected:
    IDeviceSet*_iDeviceSet;
    IDeviceSettings*subject;
};

//************************* ATTENUATOR
class AttenuatorCommand:public ReceiverCommand
{
public:
    AttenuatorCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    AttenuatorCommand();
    void execute()override;
};

//************************* PRESELECTORS

class PreselectorCommand:public ReceiverCommand
{
public:
    PreselectorCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* PREAM

class PreamplifireCommand:public ReceiverCommand
{
public:
    PreamplifireCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* ADC ENABLED

class AdcEnabledCommand:public ReceiverCommand
{
public:
    AdcEnabledCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* ADC THRESHOLD

class AdcThresholdCommand:public ReceiverCommand
{
public:
    AdcThresholdCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* POWER ON

class PowerCommandOn:public ReceiverCommand
{
public:
    PowerCommandOn(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* POWER OFF

class PowerCommandOff:public ReceiverCommand
{
public:
    PowerCommandOff(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* SETTINGS

class SettingsCommand:public ReceiverCommand
{
public:
    SettingsCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute()override;
};

//************************* START DDC1
class StartDDC1Command:public ReceiverCommand
{
public:
    StartDDC1Command(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute() override;
};

//************************* STOP DDC1

class StopDDC1Command:public ReceiverCommand
{
public:
    StopDDC1Command(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute() override;
};

//************************* RESTART

class SetDDC1TypeCommand:public ReceiverCommand
{
public:
    SetDDC1TypeCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute() override;
};

//************************* FREQ

class FrequencyCommand:public ReceiverCommand
{
public:
    FrequencyCommand(IDeviceSet*iDeviceSet,IDeviceSettings*subject);
    void execute() override;
};

//class SendStreamDDC1:public AbstractCommand
//{
//public:
//    SendStreamDDC1(IDeviceSet *deviceSet);
//    void execute();
//};
//************************* MACRO

class MacroCommand:public AbstractCommand
{
public:
    MacroCommand();
    void execute()override;
    void addCommand(ReceiverCommand*command);
    void removeCommand(ReceiverCommand*command);
private:
    QList<ReceiverCommand*>_commands;
};

#endif // COMMAND_HIERARCH_H
