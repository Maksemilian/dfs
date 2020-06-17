#ifndef WRD_COH_G35_CALLBACK_H
#define WRD_COH_G35_CALLBACK_H

#include "G35DDCAPI.h"

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "trmbl_tsip_reader.h"

using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

/*! \addtogroup receiver
 */
///@{

/*!
 * \brief Класс для получения блоков потоков DDC1,DDC2,IF,AUDIO
 *  от приемного устройства WRG35DDC в когерентном режиме.
 * В данном классе реализована только обработка блоков потока DDC1.
 */

class CohG35Callback: public ICohG35DDCDeviceSetCallback
{
    struct DDC1StreamCallbackData
    {
        ICohG35DDCDeviceSet* DeviceSet;
        unsigned int DeviceCount;
        const void** Buffers;
        unsigned int NumberOfSamples;
        unsigned int BitsPerSample;
    };
  public:
    CohG35Callback(const ShPtrRingPacketBuffer& buffer, TimeReader& timeReader);
    virtual ~CohG35Callback() = default;

    void __stdcall CohG35DDC_IFCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const short* Buffer, unsigned int NumberOfSamples,
                                        WORD MaxADCAmplitude, unsigned int ADCSamplingRate);
    /*!
     * \brief Принимает блоки данных DDC1 в отдельном потоке и
     *  создает пакет с данными класса proto::receiver::Packet
     *  содержащий временные данные когда был получен пакет , данные
     * счетчиков АЦП (ddcSampleCounter , adcPeriodCounter) ,
     *  а также некоторые данные настроек приемника.
     * После старта DDC1 пропускает все пакеты для которых не получен спутниковое
     * время time of week(количество секунд с начала недели).
     * Созданный пакет записывается в кольцевой буфер.
     */

    void __stdcall CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet* DeviceSet,
            unsigned int DeviceCount,
            const void** Buffers,
            unsigned int NumberOfSamples,
            unsigned int BitsPerSample);

    void __stdcall CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet* DeviceSet,
            unsigned int DeviceIndex,
            const float* Buffer,
            unsigned int NumberOfSamples);

    void __stdcall CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet* DeviceSet,
            unsigned int DeviceIndex,
            const float*  Buffer,
            unsigned int NumberOfSamples,
            float SlevelPeak,
            float SlevelRMS);

    void __stdcall CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet* DeviceSet,
            unsigned int DeviceIndex,
            unsigned int Type,
            const float*  Buffer,
            unsigned int NumberOfSamples);

    void resetData();

  private:
    void fillPacket(proto::receiver::Packet& packet,
                    DDC1StreamCallbackData& ddcStreamCallbackData,
                    double ddcSampleCounter,
                    unsigned long long adcPeriodCounter,
                    int counterBlockPPS,
                    quint16 currentWeekNumber);
    void showPacket(proto::receiver::Packet& packet);
  private:
    ShPtrRingPacketBuffer buffer;

    TimeReader& timeReader;

    bool isFirstBlock;
    int counterBlockPPS;
    double  currentDDCCounter;
    unsigned int currentTimeOfWeek;

};
///@}

#endif // WRD_CALLBACK_H
