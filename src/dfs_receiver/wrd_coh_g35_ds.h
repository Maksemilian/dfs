#ifndef COH_G35_DEVICE_SET_H
#define COH_G35_DEVICE_SET_H

#include "device_settings.h"

#include "wrd_callback_coh_g35.h"
#include "wrd_device_interface.h"
#include "wrd_callback.h"

#include <QPair>
#include <memory>
/*! \addtogroup receiver
 */
///@{
/*!
* \brief Класс-обертка над API WINRADIO для управления платой синхронизации.
* Реализует интерфейс IDevice
*/
class CohG35DeviceSet: public IDevice
{
  public:
    CohG35DeviceSet( ICohG35DDCDeviceSet* _deviceSet);
    virtual ~CohG35DeviceSet();
  public:
    /*!
     * \brief включение платы синхронизации
     *
     * \param state состояние включения
     * \return
     */
    bool setPower(bool state)override;
    bool setAttenuator(unsigned int attenuationLevel)override;
    bool setPreselectors(unsigned int lowFrequency, unsigned int highFrequency)override;
    bool setPreamplifierEnabled(bool state)override;
    bool setDDC1Frequency(unsigned int ddc1Frequency)override;
    bool setAdcNoiceBlankerEnabled(bool state)override;
    bool setAdcNoiceBlankerThreshold(unsigned short threshold)override;
    bool setDDC1Type(quint32 type)override;

    bool setSettings(const DeviceSettings& settings)override;
    //*****Stram DDC1
    /*!
     * \brief запуск поток ddc1
     * \param sampesPerBuffer - размер одного блока данных ddc1
     * посылаемого с приемника
     * \return
     */
    bool startDDC1(unsigned int sampesPerBuffer)override;
    /*!
     * \brief остановка потока ddc1
     * \return
     */
    bool stopDDC1()override;

    void setCallback(std::unique_ptr<CohG35Callback> callback);

    COH_G35DDC_DEVICE_SET getDeviceSetInfo();
    QString getDeviceSetName();
  private:
    void freeResource();
  private:
    ICohG35DDCDeviceSet* _deviceSet = nullptr;
    std::unique_ptr<CohG35Callback>uPtrCallback;
};
///@}
#endif // COH_G35_DEVICE_SET_H
