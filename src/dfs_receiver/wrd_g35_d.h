#ifndef WRD_G35_D_H
#define WRD_G35_D_H

#include "G35DDCAPI.h"
#include "wrd_interface.h"

class TimeReader;

class G35Device:public IDevice
{
public:
    G35Device(IG35DDCDevice*device);
    bool setPower(bool state) override;
    bool setAttenuator(unsigned int attenuationLevel)override;
    bool setPreselectors(unsigned int lowFrequency,unsigned int highFrequency)override;
    bool setPreamplifierEnabled(bool state)override;
    bool setDDC1Frequency(unsigned int ddc1Frequency)override;
    bool setAdcNoiceBlankerEnabled(bool state)override;
    bool setAdcNoiceBlankerThreshold(unsigned short threshold)override;
    bool setDDC1Type(quint32 type)override;

    bool setSettings(const DeviceSetSettings &settings)override;
    //*****Stram DDC1
    bool startDDC1(unsigned int sampesPerBuffer)override;
    bool stopDDC1()override;
 private:
    IG35DDCDevice *_device;

};

#endif // WRD_G35_D_H
