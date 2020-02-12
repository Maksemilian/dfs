#ifndef WRD_INTERFACE_H
#define WRD_INTERFACE_H

class DeviceSettings;
/*! \addtogroup receiver
 */
///@{
/*!
 * \brief интерфейс устройств
 */
class IDevice
{
  public :
    virtual  bool setPower(bool state) = 0;
    virtual  bool setAttenuator(unsigned int attenuationLevel) = 0;
    virtual  bool setPreselectors(unsigned int lowFrequency, unsigned int highFrequency) = 0;
    virtual  bool setPreamplifierEnabled(bool state) = 0;
    virtual  bool setDDC1Frequency(unsigned int ddc1Frequency) = 0;
    virtual  bool setAdcNoiceBlankerEnabled(bool state) = 0;
    virtual  bool setAdcNoiceBlankerThreshold(unsigned short threshold) = 0;
    virtual  bool setDDC1Type(unsigned int type) = 0;

    virtual  bool setSettings(const DeviceSettings& settings) = 0;
    //*****Stram DDC1
    virtual  bool startDDC1(unsigned int sampesPerBuffer) = 0;
    virtual  bool stopDDC1() = 0;
};
///@}

#endif // WRD_INTERFACE_H
