#include "command_factory.h"
#include "command_hierarch.h"

FactoryCommand::FactoryCommand()=default;

AttenuatorCommand* FactoryCommand::getAttenuatorCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new AttenuatorCommand(syncManager,subject);
}

PreselectorCommand* FactoryCommand::getPreselectorCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new PreselectorCommand(syncManager,subject);
}


PreamplifireCommand *FactoryCommand::getPreamplifireCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new PreamplifireCommand(syncManager,subject);
}

AdcEnabledCommand *FactoryCommand::getAdcEnabledCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new AdcEnabledCommand(syncManager,subject);
}

AdcThresholdCommand *FactoryCommand::getAdcThresholdCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new AdcThresholdCommand(syncManager,subject);
}

PowerCommandOn *FactoryCommand ::getPowerComandOn(IDeviceSet *syncManager, IDeviceSettings *subject)
{
     return new PowerCommandOn(syncManager,subject);
}

PowerCommandOff *FactoryCommand ::getPowerComandOff(IDeviceSet *syncManager, IDeviceSettings *subject)
{
     return new PowerCommandOff(syncManager,subject);
}

SettingsCommand *FactoryCommand::getSettingsCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new SettingsCommand(syncManager,subject);
}

StartDDC1Command *FactoryCommand::getStartDdc1Command(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new StartDDC1Command(syncManager,subject);
}

StopDDC1Command *FactoryCommand::getStopDdc1Command(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new StopDDC1Command(syncManager,subject);
}

SetDDC1TypeCommand *FactoryCommand::getSetDdc1Command(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new SetDDC1TypeCommand(syncManager,subject);
}

FrequencyCommand*FactoryCommand::getFrequencyCommand(IDeviceSet *syncManager, IDeviceSettings *subject)
{
    return new FrequencyCommand(syncManager,subject);
}

//AbstractCommand *FactoryCommand::getSendStreamDDC1()
//{
//    return new SendStreamDDC1();
//}
//SyncStartCommand*FactoryCommand::getSyncStartCommand(IDeviceSet *syncManager, IDeviceSetSettings *subject)
//{
//    return new SyncStartCommand(syncManager,subject);
//}

//SyncStopCommand*FactoryCommand::getSyncStopCommand(IDeviceSet *syncManager, IDeviceSetSettings *subject)
//{
//    return new SyncStopCommand(syncManager,subject);
//}

//AddTaskCommand*FactoryCommand::getAddTaskCommand(IDeviceSet *syncManager, IDeviceSetSettings *subject)
//{
//    return new AddTaskCommand(wd,subject);
//}

MacroCommand *FactoryCommand::getMacroCommand()
{
    return new MacroCommand();
}
