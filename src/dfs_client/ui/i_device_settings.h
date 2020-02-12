#ifndef I_DEVICE_SETTINGS_H
#define I_DEVICE_SETTINGS_H

#include "device_settings.h"

class IDeviceSettings
{
  public :
    virtual ~IDeviceSettings() = default;
    virtual bool getPower() = 0;
    virtual DeviceSettings getSettings() = 0;
    virtual quint32 getDDC1Frequency() = 0;
    virtual quint32 getDDC1Type() = 0;
    virtual quint32 getSamplesPerBuffer() = 0;
    virtual quint32 getBandwith() = 0;
    virtual quint32 getSampleRateForBandwith() = 0;
    virtual quint32 getAttenuator() = 0;
    virtual bool getPreamplifierEnabled() = 0;
    virtual Preselectors getPreselectors() = 0;

    virtual bool getAdcNoiceBlankerEnabled() = 0;
    virtual quint16 getAdcNoiceBlankerThreshold() = 0;

};
#endif // I_DEVICE_SETTINGS_H
