#ifndef WRD_G35_D_H
#define WRD_G35_D_H

#include "wrd_device.h"
#include "wrd_g35_callback.h"

class DeviceSettings;

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
    bool setDDC1Type(unsigned int type)override;

    bool setSettings(const DeviceSettings &settings)override;
    //*****Stram DDC1
    bool startDDC1(unsigned int sampesPerBuffer)override;
    bool stopDDC1()override;
    void setCallback(std::unique_ptr<G35Callback> callback);
    G35DDC_DEVICE_INFO getDeviceInfo();
 private:
    IG35DDCDevice *_device;
    std::unique_ptr<G35Callback>_callback;

};

#endif // WRD_G35_D_H
