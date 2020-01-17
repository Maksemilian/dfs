#include "command_hierarch.h"

#include "i_device_settings.h"
#include "i_deviceset.h"

#include "receiver.pb.h"
#include "client_manager.h"
AbstractCommand::AbstractCommand() {}

AbstractCommand::~AbstractCommand() = default;

StartSendingStream::StartSendingStream(ClientManager* deviceSet)
    : _iDeviceSet(deviceSet)
{

}

void StartSendingStream::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::START_SENDING_DDC1_STREAM);
    _iDeviceSet->setCommand(command);
}

StopSendingStream::StopSendingStream(ClientManager* deviceSet)
    : _iDeviceSet(deviceSet)
{

}

void StopSendingStream::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::STOP_SENDING_DDC1_STREAM);
    _iDeviceSet->setCommand(command);
}

ReceiverCommand::ReceiverCommand(ClientManager* iDeviceSet,
                                 IDeviceSettings* subject)
    : AbstractCommand(), _iDeviceSet(iDeviceSet), subject(subject)
{
}

ReceiverCommand::~ReceiverCommand() = default;

AttenuatorCommand::AttenuatorCommand(ClientManager* syncManager,
                                     IDeviceSettings* subject):
    ReceiverCommand(syncManager, subject)
{
//    qDebug()<<"Set Attenuator"<<subject->getAttenuator();

}

void AttenuatorCommand::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ATTENUATOR);
    command.set_attenuator(subject->getAttenuator());
    _iDeviceSet->setCommand(command);
}

//************************* PRES

PreselectorCommand::PreselectorCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)
{
//    qDebug()<<subject->getPreselectors();
}

void PreselectorCommand::execute()
{
    qDebug() << "Set Pres";
    proto::receiver::Command command;
    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
    proto::receiver::Preselectors* preselectors = new proto::receiver::Preselectors;
    preselectors->set_low_frequency(subject->getPreselectors().first);
    preselectors->set_high_frequency(subject->getPreselectors().second);
    command.set_command_type(proto::receiver::SET_PRESELECTORS);
    command.set_allocated_preselectors(preselectors);

    _iDeviceSet->setCommand(command);
}

//************************* PREAM

PreamplifireCommand::PreamplifireCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void PreamplifireCommand::execute()
{
    qDebug() << "Set Pream";
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_PREAMPLIFIER_ENABLED);
    command.set_preamplifier_enebled(subject->getPreamplifierEnabled());

    _iDeviceSet->setCommand(command);
}

//************************* ADC EN

AdcEnabledCommand::AdcEnabledCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void  AdcEnabledCommand::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_ENABLED);
    command.set_adc_noice_blanker_enebled(subject->getAdcNoiceBlankerEnabled());
    _iDeviceSet->setCommand(command);
}

//************************* ADC THR

AdcThresholdCommand::AdcThresholdCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void AdcThresholdCommand::execute()
{
    quint16 thr = subject->getAdcNoiceBlankerThreshold();
    proto::receiver::Command command;
    void* value = &thr;
    command.set_command_type(proto::receiver::SET_ADC_NOICE_BLANKER_THRESHOLD);
    command.set_adc_noice_blanker_threshold(value, sizeof(thr));
    _iDeviceSet->setCommand(command);
}

//************************* POWER ON

PowerCommandOn::PowerCommandOn(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void PowerCommandOn::execute()
{
    qDebug() << "Set Pream";
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_POWER_ON);
    _iDeviceSet->setCommand(command);
}

//************************* POWER OFF

PowerCommandOff::PowerCommandOff(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void PowerCommandOff::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_POWER_OFF);
    _iDeviceSet->setCommand(command);
}

//************************* SETTINGS

SettingsCommand::SettingsCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)
{
}

void SettingsCommand::execute()
{
    DeviceSettings settings = subject->getSettings();
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::CommandType::SET_SETTINGS);

    command.set_attenuator(settings.attenuator);
    //WARNING БЕЗ ДИНАМИЧЕСКОЙ ПАМЯТИ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
    proto::receiver::Preselectors* preselectors = new proto::receiver::Preselectors;
    preselectors->set_low_frequency(settings.preselectors.first);
    preselectors->set_high_frequency(settings.preselectors.second);
    command.set_allocated_preselectors(preselectors);

    command.set_preamplifier_enebled(settings.preamplifier);
    command.set_adc_noice_blanker_enebled(settings.adcEnabled);

    quint16 threshold = settings.threshold;
    void* value = &threshold;
    command.set_adc_noice_blanker_threshold(value, sizeof(threshold));

    command.set_ddc1_type(settings.ddcType);
    command.set_samples_per_buffer(settings.samplesPerBuffer);
    command.set_ddc1_frequency(settings.frequency);
    _iDeviceSet->setCommand(command);
}

//************************* START DDC

StartDDC1Command::StartDDC1Command(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void StartDDC1Command::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::START_DDC1);
    command.set_samples_per_buffer(subject->getSamplesPerBuffer());
    _iDeviceSet->setCommand(command);
}

//************************* STOP DDC

StopDDC1Command::StopDDC1Command(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void StopDDC1Command::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::STOP_DDC1);
    _iDeviceSet->setCommand(command);
}

//************************* RESTART

SetDDC1TypeCommand::SetDDC1TypeCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void SetDDC1TypeCommand::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_TYPE);
    command.set_ddc1_type(subject->getDDC1Type());
    _iDeviceSet->setCommand(command);
}

//************************* FREQ

FrequencyCommand::FrequencyCommand(ClientManager* iDeviceSet, IDeviceSettings* subject)
    : ReceiverCommand(iDeviceSet, subject)    {}

void FrequencyCommand::execute()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DDC1_FREQUENCY);
    command.set_ddc1_frequency(subject->getDDC1Frequency());
    _iDeviceSet->setCommand(command);
}

//SendStreamDDC1::SendStreamDDC1(IDeviceSet*iDeviceSet)
//{

//}
//************************* MACRO

MacroCommand::MacroCommand() {}

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
