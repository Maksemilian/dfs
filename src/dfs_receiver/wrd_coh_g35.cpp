#include "wrd_coh_g35_ds.h"
#include "wrd_coh_callback_g35.h"

#include "trmbl_tsip_reader.h"

#include <QTimer>
#include <QDebug>

/*! \addtogroup receiver
 */
///@{

extern G3XDDCAPI_CREATE_INSTANCE createInstance;
/*! \addtogroup receiver
 */
///@{

unsigned int numberAvailableDeviceSet()
{
    ICohG35DDCDeviceSetEnumerator* enumerator;
    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator)))
    {
        qDebug() << "Enumerator error";
        return 0;
    }
    unsigned int number = enumerator->GetDeviceSetCount();
    enumerator->Release();
    return number;
}

ShPtrDevice createCohG35Device(unsigned int deviceSetIndex,
                               const ShPtrRingBuffer& buffer,
                               bool demoMode)
{
    qDebug() << "CohG35DeviceCreator::createDevice";
    ICohG35DDCDeviceSetEnumerator* enumerator;
    ICohG35DDCDeviceSet* deviceSet;

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator)))
    {
        qDebug() << "Enumerator error";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET,
                       reinterpret_cast<void**>(&deviceSet)))
    {
        qDebug() << "DeviceSet error";
        return nullptr;
    }

    quint32 countDeviceInSet = 0;
    G35DDC_DEVICE_INFO* deviceInfoMas = nullptr;
    if(!demoMode)
    {
        enumerator->GetDeviceSetInfo(deviceSetIndex, nullptr, &countDeviceInSet);
        deviceInfoMas = new G35DDC_DEVICE_INFO[countDeviceInSet];
        enumerator->GetDeviceSetInfo(deviceSetIndex, deviceInfoMas, &countDeviceInSet);
    }
    else
    {
        qDebug() << "DEMO_MODE_SELECTED";
        countDeviceInSet = 1;
        deviceInfoMas = dynamic_cast<G35DDC_DEVICE_INFO*>(G35DDC_OPEN_DEMO_SET);
    }
    if(deviceSet->Open(deviceInfoMas, countDeviceInSet))
    {
        qDebug() << "Device Set" << deviceSetIndex << "- count device:" << countDeviceInSet;
        if(!demoMode)
        {
            for(quint32 deviceIndex = 0; deviceIndex < countDeviceInSet; deviceIndex++)
            {
                qDebug() << "|_____SerialNumber:" << deviceInfoMas[deviceIndex].SerialNumber;
            }

            delete []deviceInfoMas;
        }
        enumerator->Release();

        std::shared_ptr<CohG35DeviceSet> shPtr =
            std::make_shared<CohG35DeviceSet>(deviceSet);

        shPtr->setCallback(std::make_unique<CohG35Callback>(buffer,
                           TimeReader::instance()));
        return static_cast<std::shared_ptr<IDevice>>(shPtr);
    }
    qDebug() << "Device is not open";
    delete [] deviceInfoMas;

    enumerator->Release();
    deviceSet->Release();
    return nullptr;
}

CohG35DeviceSet::CohG35DeviceSet(ICohG35DDCDeviceSet* deviceSet):
    _deviceSet(deviceSet) {}

void CohG35DeviceSet::setCallback(std::unique_ptr<ICohG35DDCDeviceSetCallback> callback)
{
    if(!callback.get())
    {
        return;
    }

    uPtrCallback = std::move(callback);

    _deviceSet->SetCallback(uPtrCallback.get());
}

QString CohG35DeviceSet::getDeviceSetName()
{
    quint32 deviceCount;
    _deviceSet->GetDeviceCount(&deviceCount);
    G35DDC_DEVICE_INFO deviceInfo[deviceCount];
    _deviceSet->GetDeviceInfos(deviceInfo, &deviceCount);
    QString deviceSetName = "DS#";
    for(quint32 i = 0; i < deviceCount; i++)
    {
        deviceSetName += deviceInfo[i].SerialNumber;
        if(i != deviceCount - 1)
        {
            deviceSetName += "_";
        }
    }

    return deviceSetName;
}

void CohG35DeviceSet::freeResource()
{
    if(_deviceSet != nullptr)
    {
        _deviceSet->Release();
    }
}

CohG35DeviceSet::~CohG35DeviceSet()
{
    freeResource();
}

COH_G35DDC_DEVICE_SET CohG35DeviceSet::getDeviceSetInfo()
{
    COH_G35DDC_DEVICE_SET deviceSetInfos;
    _deviceSet->GetDeviceCount(&deviceSetInfos.DeviceCount);
    deviceSetInfos.DeviceInfo = new G35DDC_DEVICE_INFO[deviceSetInfos.DeviceCount];
    _deviceSet->GetDeviceInfos(deviceSetInfos.DeviceInfo, &deviceSetInfos.DeviceCount);
    return deviceSetInfos;
}

//**************COMMAND TO DEVICE SET

bool CohG35DeviceSet::setPower(bool state)
{
    return _deviceSet->SetPower(state);
}

bool CohG35DeviceSet::setAttenuator(unsigned int attenuationLevel)
{
    return _deviceSet->SetAttenuator(attenuationLevel);
}

bool CohG35DeviceSet::setPreamplifierEnabled(bool state)
{
    return _deviceSet->SetPreamp(state);
}

bool CohG35DeviceSet::setPreselectors(unsigned int lowFrequency, unsigned int highFrequency)
{
    return _deviceSet->SetPreselectors(lowFrequency, highFrequency);
}

bool CohG35DeviceSet::setAdcNoiceBlankerEnabled(bool state)
{
    return _deviceSet->SetADCNoiseBlanker(state);
}

bool CohG35DeviceSet::setAdcNoiceBlankerThreshold(unsigned short threshold)
{
    return _deviceSet->SetADCNoiseBlankerThreshold(threshold);
}

bool CohG35DeviceSet::setDDC1Frequency(unsigned int ddc1Frequency)
{
    return _deviceSet->SetDDC1Frequency(ddc1Frequency);
}

bool CohG35DeviceSet::setDDC1Type(quint32 type)
{
    return  _deviceSet->SetDDC1(type);
}

bool CohG35DeviceSet::setSettings(const DeviceSettings& settings)
{
    bool succesed = false;

    succesed = setAttenuator(settings.attenuator);
    qDebug() << "======Comand  SET_ATTENUATOR " << settings.attenuator << "Db" << "|| Succesed command" << succesed;

    succesed = setPreselectors(settings.preselectors.first, settings.preselectors.second);
    qDebug() << "======Comand  SET_PRESELECTORS" << "Frequency:" << "Low " << settings.preselectors.first << "Hz"
             << "High " << settings.preselectors.second << "Hz" << "|| Succesed command" << succesed;

    succesed = setPreamplifierEnabled(settings.preamplifier);
    qDebug() << "======Comand  SET_PREAMPLIFIED_ENABLED" << settings.preamplifier << "|| Succesed command" << succesed;

    succesed = setAdcNoiceBlankerEnabled(settings.adcEnabled);
    qDebug() << "======Comand  SET_ADC_NOICE_BLANKER_ENABLED" << settings.adcEnabled << "|| Succesed command" << succesed;

    succesed = setAdcNoiceBlankerThreshold(settings.threshold);
    qDebug() << "======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD" << settings.threshold << "|| Succesed command" << succesed;

    succesed = setDDC1Frequency(settings.frequency);
    qDebug() << "======Comand  SET_DDC1_FREQUENCY" << settings.frequency << "|| Succesed command" << succesed;

    succesed = _deviceSet->SetDDC1(settings.ddcType);
    qDebug() << "======Comand  SET_DDC1_TYPE" << settings.ddcType << "|| Succesed command" << succesed;

    return succesed;
}

bool CohG35DeviceSet::startDDC1(unsigned int samplesPerBuffer)
{
    Q_ASSERT_X(_deviceSet, "deviceSet is null", "CohG35DeviceSet::startDDC1();");
    bool success = false;
    qDebug() << "****CALL_BACK" << uPtrCallback.get();
//    if(!timeReader.isStarted())return false;
    if(_deviceSet && uPtrCallback.get())
    {
        dynamic_cast<CohG35Callback*>(uPtrCallback.get())->resetData();
        success = _deviceSet->StartDDC1(samplesPerBuffer);
//        resetData();
//        uPtrCallback->resetData();
//        timeReader.getTime(currentWeekNumber,currentTimeOfWeek);
    }
    return success;
}


bool CohG35DeviceSet::stopDDC1()
{
    bool success = false;
    if(_deviceSet)
    {
        success = _deviceSet->StopDDC1();
//        buffer->reset();
    }
    return success;
}

/*
bool CohG35DeviceSet::setShiftPhaseDDC1(unsigned int deviceIndex, double phaseShift)
{
    return _deviceSet->SetDDC1PhaseShift(deviceIndex,phaseShift);
}
*/
///@}
