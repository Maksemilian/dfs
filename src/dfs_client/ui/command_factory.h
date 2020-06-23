#ifndef FACTORY_COMMAND_H
#define FACTORY_COMMAND_H


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
