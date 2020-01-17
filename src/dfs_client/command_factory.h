#ifndef FACTORY_COMMAND_H
#define FACTORY_COMMAND_H
#include "QObject"
class IDeviceSettings ;
class ClientManager;
class AbstractCommand;

class WidgetDirector;
class MacroCommand;

class PreselectorCommand;
class PreamplifireCommand;
class AdcEnabledCommand;
class AdcThresholdCommand;
class AttenuatorCommand;
class PowerCommandOn;
class PowerCommandOff;

class SettingsCommand;
class StartDDC1Command;
class StopDDC1Command;
class SetDDC1TypeCommand;
class FrequencyCommand;
class SyncStartCommand;
class SyncStopCommand;
class AddTaskCommand;

class SendStreamDDC1;
class ClientManager;
/*
class AbstractCommand: public QObjectUserData
{
  public:
    AbstractCommand() = default;
    ~AbstractCommand() = default;
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

class MacroCommand: public AbstractCommand
{
  public:
    void execute()override;
    void addCommand(AbstractCommand* command);
    void removeCommand(AbstractCommand* command);
  private:
    QList<AbstractCommand*>_commands;
};
*/
class FactoryCommand
{
    FactoryCommand();

  public:
    static FactoryCommand& intstance()
    {
        static FactoryCommand factory;
        return factory;
    }

    static AbstractCommand* getPreselectorCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getPreamplifireCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getAdcEnabledCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getAdcThresholdCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getAttenuatorCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getPowerComandOn(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getPowerComandOff(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getSettingsCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getStartDdc1Command(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getStopDdc1Command(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getSetDdc1Command(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getFrequencyCommand(ClientManager* clientManager,
            IDeviceSettings* subject);

    static AbstractCommand* getStartSendingStreamCommand(ClientManager* clientManager);
    static AbstractCommand* getStopSendingStreamCommand(ClientManager* clientManager);

    static MacroCommand* getMacroCommand();
};

#endif // FACTORY_COMMAND_H
