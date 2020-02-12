#ifndef COMMAND_HIERARCH_H
#define COMMAND_HIERARCH_H

#include <QObjectUserData>

#include <QDebug>

class IDeviceSettings;
class ClientManager;

#include "receiver.pb.h"

class AbstractCommand: public QObjectUserData
{
  public:
    AbstractCommand();
    virtual ~AbstractCommand();
    virtual void execute() = 0;
};

template <class Receiver, class Command>
class SimpleCommand : public AbstractCommand
{
  public:
    typedef void (Receiver::* Action) (Command&);
    SimpleCommand(Receiver* r, Action a, Command c) :
        _receiver(r), _action(a), _c(c) { }
    virtual void execute()
    {
        (_receiver->*_action)(_c);
    }
  private:
    Action _action;
    Receiver* _receiver;
    Command _c;
};

class StartSendingStream: public AbstractCommand
{
  public:
    StartSendingStream(ClientManager* deviceSet);
    void execute()override;
  private:
    ClientManager* _iDeviceSet;
};

class StopSendingStream: public AbstractCommand
{
  public:
    StopSendingStream(ClientManager* deviceSet);
    void execute()override;
  private:
    ClientManager* _iDeviceSet;
};


class ReceiverCommand: public AbstractCommand
{
  public:
    ReceiverCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    ~ReceiverCommand();
  protected:
    ClientManager* _iDeviceSet;
    IDeviceSettings* subject;
};



//************************* ATTENUATOR
class AttenuatorCommand: public ReceiverCommand
{
  public:
    AttenuatorCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    AttenuatorCommand();
    void execute()override;
};

//************************* PRESELECTORS

class PreselectorCommand: public ReceiverCommand
{
  public:
    PreselectorCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* PREAM

class PreamplifireCommand: public ReceiverCommand
{
  public:
    PreamplifireCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* ADC ENABLED

class AdcEnabledCommand: public ReceiverCommand
{
  public:
    AdcEnabledCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* ADC THRESHOLD

class AdcThresholdCommand: public ReceiverCommand
{
  public:
    AdcThresholdCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* POWER ON

class PowerCommandOn: public ReceiverCommand
{
  public:
    PowerCommandOn(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* POWER OFF

class PowerCommandOff: public ReceiverCommand
{
  public:
    PowerCommandOff(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* SETTINGS

class SettingsCommand: public ReceiverCommand
{
  public:
    SettingsCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute()override;
};

//************************* START DDC1
class StartDDC1Command: public ReceiverCommand
{
  public:
    StartDDC1Command(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute() override;
};

//************************* STOP DDC1

class StopDDC1Command: public ReceiverCommand
{
  public:
    StopDDC1Command(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute() override;
};

//************************* RESTART

class SetDDC1TypeCommand: public ReceiverCommand
{
  public:
    SetDDC1TypeCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute() override;
};

//************************* FREQ

class FrequencyCommand: public ReceiverCommand
{
  public:
    FrequencyCommand(ClientManager* iDeviceSet, IDeviceSettings* subject);
    void execute() override;
};

//************************* MACRO

class MacroCommand: public AbstractCommand
{
  public:
    MacroCommand();
    void execute()override;
    void addCommand(AbstractCommand* command);
    void removeCommand(AbstractCommand* command);
  private:
    QList<AbstractCommand*>_commands;
};

#endif // COMMAND_HIERARCH_H
