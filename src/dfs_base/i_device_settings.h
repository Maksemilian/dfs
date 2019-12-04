#ifndef IGETSETTINGS_H
#define IGETSETTINGS_H


#include "device_settings.h"

class IDeviceSetSettings
{
public :
    virtual ~IDeviceSetSettings()=default;
    virtual bool getPower()=0;
    virtual DeviceSettings getSettings()=0;
    virtual quint32 getDDC1Frequency()=0;
    virtual quint32 getDDC1Type()=0;
    virtual quint32 getSamplesPerBuffer()=0;
    virtual quint32 getBandwith()=0;
    virtual quint32 getSampleRateForBandwith()=0;
    virtual quint32 getAttenuator()=0;
    virtual bool getPreamplifierEnabled()=0;
    virtual Preselectors getPreselectors()=0;

    virtual bool getAdcNoiceBlankerEnabled()=0;
    virtual quint16 getAdcNoiceBlankerThreshold()=0;

};
#endif // IGETSETTINGS_H
