#ifndef FACTORY_COMMAND_H
#define FACTORY_COMMAND_H

class IDeviceSetSettings ;
class WidgetDirector;
class MacroCommand;

class SyncManager;

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

    static PreselectorCommand *getPreselectorCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static PreamplifireCommand *getPreamplifireCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static AdcEnabledCommand *getAdcEnabledCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static AdcThresholdCommand *getAdcThresholdCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static AttenuatorCommand *getAttenuatorCommand(SyncManager *syncManager,  IDeviceSetSettings *subject);

    static PowerCommandOn *getPowerComandOn(SyncManager *syncManager,IDeviceSetSettings *subject);
    static PowerCommandOff *getPowerComandOff(SyncManager *syncManager,IDeviceSetSettings *subject);

    static SettingsCommand *getSettingsCommand(SyncManager *syncManager,IDeviceSetSettings *subject);

    static StartDDC1Command *getStartDdc1Command(SyncManager *syncManager,IDeviceSetSettings *subject);
    static StopDDC1Command *getStopDdc1Command(SyncManager *syncManager,IDeviceSetSettings *subject);
    static SetDDC1TypeCommand *getSetDdc1Command(SyncManager *syncManager,IDeviceSetSettings *subject);
    static FrequencyCommand *getFrequencyCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static SyncStartCommand *getSyncStartCommand(SyncManager *syncManager,IDeviceSetSettings *subject);
    static SyncStopCommand *getSyncStopCommand(SyncManager *syncManager,IDeviceSetSettings *subject);

    static AddTaskCommand *getAddTaskCommand(WidgetDirector*wd,IDeviceSetSettings*subject);

    static MacroCommand *getMacroCommand();

};

#endif // FACTORY_COMMAND_H
