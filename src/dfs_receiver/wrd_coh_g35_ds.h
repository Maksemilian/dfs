#ifndef COH_G35_DEVICE_SET_H
#define COH_G35_DEVICE_SET_H

#include "wrd_coh_callback_g35.h"
#include "wrd_device_interface.h"

#include "receiver.pb.h"
#include "ring_buffer.h"

#include <memory>

/*! \addtogroup receiver
 */
///@{

using ShPtrDevice = std::shared_ptr<IDevice>;
using ShPtrRingBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

/*!
* \brief Класс-обертка над API WINRADIO
* для управления набором приемников WRG35DDC(в когерентном режиме)
* подключенных к одной плате синхронизации.
*/
class CohG35DeviceSet: public IDevice
{
  public:
    CohG35DeviceSet( ICohG35DDCDeviceSet* _deviceSet);
    virtual ~CohG35DeviceSet();
  public:
    /*!
     * \brief включение набора приемников
     */
    bool setPower(bool state)override;
    bool setAttenuator(unsigned int attenuationLevel)override;
    bool setPreselectors(unsigned int lowFrequency, unsigned int highFrequency)override;
    bool setPreamplifierEnabled(bool state)override;

    /*!
     * \brief устанавливает настройку частоты DDC1.
     * частот не должна быть больше 50 Мгц
     */
    bool setDDC1Frequency(unsigned int ddc1Frequency)override;
    bool setAdcNoiceBlankerEnabled(bool state)override;
    bool setAdcNoiceBlankerThreshold(unsigned short threshold)override;

    /*!
     * \brief устанавливает тип DDC1.Всего типов 33.
     * Каждый тип определяет полосу пропускания и частоту дескритизации
     * канала приемника.
     */
    bool setDDC1Type(quint32 type)override;

    /*!
     * \brief установка настроек для платы синхронизации
     * \param settings
     * \return
     */
    bool setSettings(const DeviceSettings& settings)override;

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

    void setCallback(std::unique_ptr<ICohG35DDCDeviceSetCallback> callback);

    COH_G35DDC_DEVICE_SET getDeviceSetInfo();
    QString getDeviceSetName();
  private:
    void freeResource();
  private:
    ICohG35DDCDeviceSet* _deviceSet = nullptr;
    std::unique_ptr<ICohG35DDCDeviceSetCallback>uPtrCallback;
};

/*!
 * \brief createCohG35Device создает устройство платы синхронизации
 * \param deviceIndex индекс платы синхронизации
 * \param buffer кольцевой буфер для хранения данных ddc1 потока
 * \return устройство
 */
ShPtrDevice createCohG35Device(unsigned int deviceIndex,
                               const ShPtrRingBuffer& buffer,
                               bool demo_mode = false);

/*!
 * \brief возвращает количество доступных наборов приемников
 */
unsigned int numberAvailableDeviceSet();
///@}
#endif // COH_G35_DEVICE_SET_H
