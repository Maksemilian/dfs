#include "wrd_g35_d.h"

G35Device::G35Device(IG35DDCDevice*device):_device(device)
{

}
bool G35Device::setPower(bool state)
{
    return _deviceSet->SetPower(state);
}

bool G35Device::setAttenuator(unsigned int attenuationLevel)
{
    return _deviceSet->SetAttenuator(attenuationLevel);
}

bool G35Device::setPreamplifierEnabled(bool state)
{
    return _deviceSet->SetPreamp(state);
}

bool G35Device::setPreselectors(unsigned int lowFrequency, unsigned int highFrequency)
{
    return _deviceSet->SetPreselectors(lowFrequency,highFrequency);
}

bool G35Device::setAdcNoiceBlankerEnabled(bool state)
{
    return _deviceSet->SetADCNoiseBlanker(state);
}

bool G35Device::setAdcNoiceBlankerThreshold(unsigned short threshold)
{
    return _deviceSet->SetADCNoiseBlankerThreshold(threshold);
}

bool G35Device::setDDC1Frequency(unsigned int ddc1Frequency)
{
    return _deviceSet->SetDDC1Frequency(ddc1Frequency);
}

bool G35Device::setDDC1Type(quint32 type)
{
    return  _deviceSet->SetDDC1(type);
}

bool G35Device::setSettings(const DeviceSetSettings &settings)
{
    bool succesed=false;

    succesed=setAttenuator(settings.attenuator);
    qDebug()<<"======Comand  SET_ATTENUATOR "<<settings.attenuator<<"Db"<<"|| Succesed command"<<succesed;

    succesed=setPreselectors(settings.preselectors.first,settings.preselectors.second);
    qDebug()<<"======Comand  SET_PRESELECTORS"<<"Frequency:"<<"Low "<<settings.preselectors.first<<"Hz"
           <<"High "<<settings.preselectors.second<<"Hz"<<"|| Succesed command"<<succesed;

    succesed=setPreamplifierEnabled(settings.preamplifier);
    qDebug()<<"======Comand  SET_PREAMPLIFIED_ENABLED"<<settings.preamplifier<<"|| Succesed command"<<succesed;

    succesed=setAdcNoiceBlankerEnabled(settings.adcEnabled);
    qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_ENABLED"<<settings.adcEnabled<<"|| Succesed command"<<succesed;

    succesed=setAdcNoiceBlankerThreshold(settings.threshold);
    qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD"<<settings.threshold<<"|| Succesed command"<<succesed;

    succesed=setDDC1Frequency(settings.frequency);
    qDebug()<<"======Comand  SET_DDC1_FREQUENCY"<<settings.frequency<<"|| Succesed command"<<succesed;

    succesed=_deviceSet->SetDDC1(settings.ddcType);
    qDebug()<<"======Comand  SET_DDC1_TYPE"<<settings.ddcType<<"|| Succesed command"<<succesed;

    return succesed;
}

bool G35Device::startDDC1(unsigned int samplesPerBuffer)
{
    Q_ASSERT_X(_deviceSet,"deviceSet is null","CohG35DeviceSet::startDDC1();");
    bool success=false;
    if(!timeReader.isStarted())return false;
    if(_deviceSet){
        resetData();
//        uPtrCallback->resetData();
        timeReader.getTime(currentWeekNumber,currentTimeOfWeek);
        success=_deviceSet->StartDDC1(samplesPerBuffer);
    }
    return success;
}


bool G35Device::stopDDC1()
{
    bool success=false;
    if(_deviceSet){
        success= _deviceSet->StopDDC1();
        buffer->reset();
    }
    return success;
}
