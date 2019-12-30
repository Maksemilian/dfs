#ifndef WRD_COH_G35_CALLBACK_H
#define WRD_COH_G35_CALLBACK_H

#include "G35DDCAPI.h"

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "trmbl_tsip_reader.h"
#include "wrd_callback.h"

using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

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
    void __stdcall CohG35DDC_IFCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const short* Buffer, unsigned int NumberOfSamples,
                                        WORD MaxADCAmplitude, unsigned int ADCSamplingRate);
    void __stdcall CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const float* Buffer, unsigned int NumberOfSamples);
    void __stdcall CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex,
            const float*  Buffer, unsigned int NumberOfSamples, float SlevelPeak, float SlevelRMS);
    void __stdcall CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, unsigned int Type,
            const float*  Buffer, unsigned int NumberOfSamples);

    void __stdcall CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceCount, const void** Buffers, unsigned int NumberOfSamples,
            unsigned int BitsPerSample);
    void resetData();
    inline ShPtrRingPacketBuffer ddc1Buffer()
    {
        return buffer;
    }
  private:
    void fillPacket(proto::receiver::Packet& packet,
                    DDC1StreamCallbackData& ddcStreamCallbackData,
                    double ddcSampleCounter,
                    unsigned long long adcPeriodCounter,
                    int counterBlockPPS);
    void showPacket(proto::receiver::Packet& packet);
  private:
    ShPtrRingPacketBuffer buffer;
    TimeReader& timeReader;

    bool isFirstBlock;
    int counterBlockPPS;
    double  currentDDCCounter;
    unsigned short currentWeekNumber;
    unsigned int currentTimeOfWeek;

    int lastBlockNumber = 0;
};


#endif // WRD_CALLBACK_H
