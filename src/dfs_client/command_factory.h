#ifndef FACTORY_COMMAND_H
#define FACTORY_COMMAND_H

class IDeviceSetSettings ;
class IDeviceSet;

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

class FactoryCommand
{
    FactoryCommand();

public:
    static FactoryCommand &intstance()
    {
        static FactoryCommand factory;
        return factory;
    }

    static PreselectorCommand *getPreselectorCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static PreamplifireCommand *getPreamplifireCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static AdcEnabledCommand *getAdcEnabledCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static AdcThresholdCommand *getAdcThresholdCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static AttenuatorCommand *getAttenuatorCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);

    static PowerCommandOn *getPowerComandOn(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static PowerCommandOff *getPowerComandOff(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);

    static SettingsCommand *getSettingsCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);

    static StartDDC1Command *getStartDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static StopDDC1Command *getStopDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static SetDDC1TypeCommand *getSetDdc1Command(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
    static FrequencyCommand *getFrequencyCommand(IDeviceSet *iDeviceSet,  IDeviceSetSettings *subject);
//    static SyncStartCommand *getSyncStartCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
//    static SyncStopCommand *getSyncStopCommand(SyncManager *syncManager,IDeviceSetSettings *subject);

//    static AddTaskCommand *getAddTaskCommand(WidgetDirector*wd,IDeviceSetSettings*subject);

    static MacroCommand *getMacroCommand();

};

#endif // FACTORY_COMMAND_H
