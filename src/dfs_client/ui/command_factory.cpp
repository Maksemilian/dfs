#include "command_factory.h"
#include "command_hierarch.h"
#include "i_device_settings.h"
#include "client_manager.h"

/*
void MacroCommand::execute()
{
    qDebug() << "====================MackroCommand EXEC";

    for(AbstractCommand* rc : _commands)
    {
        rc->execute();
    }
}

void MacroCommand::addCommand(AbstractCommand* command)
{
    _commands << command;
}

void MacroCommand::removeCommand(AbstractCommand* command)
{
    _commands.removeOne(command);
}

*/
FactoryCommand::FactoryCommand() = default;

AbstractCommand* FactoryCommand::getAttenuatorCommand(ClientManager* clientManager,
        IDeviceSettings* subject)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_ATTENUATOR);
//    command.set_attenuator(subject->getAttenuator());

////    return   new SimpleCommand<ClientManager,
////             proto::receiver::Command>(clientManager,
////                                       &ClientManager::setCommand,
////                                       command);

    return new AttenuatorCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getPreselectorCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
//    qDebug() << "Set Pres";
//    proto::receiver::Command command;
//    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
//    proto::receiver::Preselectors* preselectors = new proto::receiver::Preselectors;
//    preselectors->set_low_frequency(subject->getPreselectors().first);
//    preselectors->set_high_frequency(subject->getPreselectors().second);
//    command.set_command_type(proto::receiver::SET_PRESELECTORS);
//    command.set_allocated_preselectors(preselectors);

//    return   new SimpleCommand<ClientManager,
//             proto::receiver::Command>(clientManager,
//                                       &ClientManager::setCommand,
//                                       command);
    return new PreselectorCommand(clientManager, subject);
}


AbstractCommand* FactoryCommand::getPreamplifireCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
//    qDebug() << "Set Pream";
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_PREAMPLIFIER_ENABLED);
//    command.set_preamplifier_enebled(subject->getPreamplifierEnabled());
////    return new SimpleCommand<ClientManager,
////           proto::receiver::Command>(clientManager,
////                                     &ClientManager::setCommand,
////                                     command);
    return new PreamplifireCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getAdcEnabledCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_ENABLED);
//    command.set_adc_noice_blanker_enebled(subject->getAdcNoiceBlankerEnabled());
////    return  new SimpleCommand<ClientManager,
////            proto::receiver::Command>(clientManager,
////                                      &ClientManager::setCommand,
////                                      command);
    return new AdcEnabledCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getAdcThresholdCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
//    quint16 thr = subject->getAdcNoiceBlankerThreshold();
//    proto::receiver::Command command;
//    void* value = &thr;
//    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_THRESHOLD);
//    command.set_adc_noice_blanker_threshold(value, sizeof(thr));
////    return new SimpleCommand<ClientManager,
////           proto::receiver::Command>(clientManager,
////                                     &ClientManager::setCommand,
////                                     command);
    return new AdcThresholdCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand ::getPowerComandOn(ClientManager* clientManager,
        IDeviceSettings* subject)
{
//    Q_UNUSED(subject);
//    qDebug() << "Set Pream";
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_POWER_ON);
////    return new SimpleCommand<ClientManager,
////           proto::receiver::Command>(clientManager,
////                                     &ClientManager::setCommand,
////                                     command);
    return new PowerCommandOn(clientManager, subject);
}

AbstractCommand* FactoryCommand ::getPowerComandOff(ClientManager* clientManager,
        IDeviceSettings* subject)
{
    /*    Q_UNUSED(subject);
        proto::receiver::Command command;
        command.set_command_type(proto::receiver::SET_POWER_OFF);
    //    return new SimpleCommand<ClientManager,
    //           proto::receiver::Command>(clientManager,
    //                                     &ClientManager::setCommand,
    //                                     command)*/;
    return new PowerCommandOff(clientManager, subject);
}

AbstractCommand* FactoryCommand::getSettingsCommand(ClientManager* clientManager, IDeviceSettings* subject)
{
//    DeviceSettings settings = subject->getSettings();
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::CommandType::SET_SETTINGS);

//    command.set_attenuator(settings.attenuator);
//    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
//    proto::receiver::Preselectors* preselectors = new proto::receiver::Preselectors;
//    preselectors->set_low_frequency(settings.preselectors.first);
//    preselectors->set_high_frequency(settings.preselectors.second);
//    command.set_allocated_preselectors(preselectors);

//    command.set_preamplifier_enebled(settings.preamplifier);
//    command.set_adc_noice_blanker_enebled(settings.adcEnabled);

//    quint16 threshold = settings.threshold;
//    void* value = &threshold;
//    command.set_adc_noice_blanker_threshold(value, sizeof(threshold));

//    command.set_ddc1_type(settings.ddcType);
//    command.set_samples_per_buffer(settings.samplesPerBuffer);
//    command.set_ddc1_frequency(settings.frequency);
    return new SettingsCommand(clientManager, subject);
//    return new SimpleCommand<ClientManager,
//           proto::receiver::Command>(clientManager,
//                                     &ClientManager::setCommand,
//                                     command);
}

AbstractCommand* FactoryCommand::getStartDdc1Command(ClientManager* clientManager, IDeviceSettings* subject)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::START_DDC1);
//    command.set_samples_per_buffer(subject->getSamplesPerBuffer());
//    return new SimpleCommand<ClientManager,
//           proto::receiver::Command>(clientManager,
//                                     &ClientManager::setCommand,
//                                     command);
    return new StartDDC1Command(clientManager, subject);
}

AbstractCommand* FactoryCommand::getStopDdc1Command(ClientManager* clientManager, IDeviceSettings* subject)
{
//    Q_UNUSED(subject);
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::STOP_DDC1);
//    return new SimpleCommand<ClientManager,
//           proto::receiver::Command>(clientManager,
//                                     &ClientManager::setCommand,
//                                     command);
    return new StopDDC1Command(clientManager, subject);
}

AbstractCommand* FactoryCommand::getSetDdc1Command(ClientManager* clientManager,
        IDeviceSettings* subject)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_DDC1_TYPE);
//    command.set_ddc1_type(subject->getDDC1Type());
//    return new SimpleCommand<ClientManager,
//           proto::receiver::Command>(clientManager,
//                                     &ClientManager::setCommand,
//                                     command);
    return new SetDDC1TypeCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getFrequencyCommand(ClientManager* clientManager,
        IDeviceSettings* subject)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_DDC1_FREQUENCY);
//    command.set_ddc1_frequency(subject->getDDC1Frequency());
//    return new SimpleCommand<ClientManager,
//           proto::receiver::Command>(clientManager,
//                                     &ClientManager::setCommand,
//                                     command);
    return new FrequencyCommand(clientManager, subject);
}

AbstractCommand* FactoryCommand::getStartSendingStreamCommand(ClientManager* clientManager)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::START_SENDING_DDC1_STREAM);
////    return new SimpleCommand<ClientManager,
////           proto::receiver::Command>(clientManager,
////                                     &ClientManager::setCommand,
////                                     command);
    return new StartSendingStream(clientManager);
}

AbstractCommand* FactoryCommand::getStopSendingStreamCommand(ClientManager* clientManager)
{
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::STOP_SENDING_DDC1_STREAM);
////    return new SimpleCommand<ClientManager,
////           proto::receiver::Command>(clientManager,
////                                     &ClientManager::setCommand,
////                                     command);
    return new StopSendingStream(clientManager);
}


MacroCommand* FactoryCommand::getMacroCommand()
{
    return new MacroCommand();
}
