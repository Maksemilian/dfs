#include "command_factory.h"
#include "command_hierarch.h"
#include "i_device_settings.h"
#include "client_manager.h"

FactoryCommand::FactoryCommand() = default;

AbstractCommand* FactoryCommand::getAttenuatorCommand(ClientManager* clientManager,
        IDeviceSettings* subject)
{
    return new AttenuatorCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getPreselectorCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new PreselectorCommand(clientManager, subject);
}


AbstractCommand* FactoryCommand::getPreamplifireCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new PreamplifireCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getAdcEnabledCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new AdcEnabledCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getAdcThresholdCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new AdcThresholdCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand ::getPowerComandOn(ClientManager* clientManager,
        IDeviceSettings* subject)
{
    return new PowerCommandOn(clientManager, subject);
}

AbstractCommand* FactoryCommand ::getPowerComandOff(ClientManager* clientManager,
        IDeviceSettings* subject)
{
    return new PowerCommandOff(clientManager, subject);
}

AbstractCommand* FactoryCommand::getSettingsCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new SettingsCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getStartDdc1Command(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new StartDDC1Command(clientManager, subject);
}

AbstractCommand* FactoryCommand::getStopDdc1Command(ClientManager* clientManager, IDeviceSettings* subject)
{
    return new StopDDC1Command(clientManager, subject);
}

AbstractCommand* FactoryCommand::getSetDdc1Command(ClientManager* clientManager,
        IDeviceSettings* subject)
{
    return new SetDDC1TypeCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getFrequencyCommand(ClientManager* clientManager,
        IDeviceSettings* subject)
{

    return new FrequencyCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getStartSendingStreamCommand(ClientManager* clientManager)
{
    return new StartSendingStream(clientManager);
}

AbstractCommand* FactoryCommand::getStopSendingStreamCommand(ClientManager* clientManager)
{
    return new StopSendingStream(clientManager);
}


MacroCommand* FactoryCommand::getMacroCommand()
{
    return new MacroCommand();
}
