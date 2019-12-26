#ifndef FACTORY_COMMAND_H
#define FACTORY_COMMAND_H

class IDeviceSettings ;
class IDeviceSet;
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

class FactoryCommand
{
    FactoryCommand();

public:
    static FactoryCommand &intstance()
    {
        static FactoryCommand factory;
        return factory;
    }

    static PreselectorCommand *getPreselectorCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static PreamplifireCommand *getPreamplifireCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static AdcEnabledCommand *getAdcEnabledCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static AdcThresholdCommand *getAdcThresholdCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static AttenuatorCommand *getAttenuatorCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);

    static PowerCommandOn *getPowerComandOn(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static PowerCommandOff *getPowerComandOff(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);

    static SettingsCommand *getSettingsCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);

    static StartDDC1Command *getStartDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static StopDDC1Command *getStopDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static SetDDC1TypeCommand *getSetDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);
    static FrequencyCommand *getFrequencyCommand(IDeviceSet *iDeviceSet,  IDeviceSettings *subject);

    static MacroCommand *getMacroCommand();
};

#endif // FACTORY_COMMAND_H
