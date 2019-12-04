#ifndef WRD_CALLBACK_H
#define WRD_CALLBACK_H

#include "G35DDCAPI.h"
#include "ring_buffer.h"
#include "receiver.pb.h"
#include "trmbl_tsip_reader.h"

#include <memory>
class TimeReader;
using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class CallbackFactory
{
public:
    static std::unique_ptr<CohG35Callback> cohG35CallbackInstance(const ShPtrRingPacketBuffer&ddc1Buffer)
    {
        return std::make_unique<CohG35Callback>(ddc1Buffer,TimeReader::instance());
    }
};

class CohG35Callback:public ICohG35DDCDeviceSetCallback
{
    struct DDC1StreamCallbackData{
        ICohG35DDCDeviceSet *DeviceSet;
        unsigned int DeviceCount;
        const void **Buffers;
        unsigned int NumberOfSamples;
        unsigned int BitsPerSample;
    };
public:
    CohG35Callback(const ShPtrRingPacketBuffer &buffer,const TimeReader&timeReader);
    void __stdcall CohG35DDC_IFCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,const short *Buffer,unsigned int NumberOfSamples,
                                        WORD MaxADCAmplitude,unsigned int ADCSamplingRate);
    void __stdcall CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,const float *Buffer,unsigned int NumberOfSamples);
    void __stdcall CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,
                                                            const float  *Buffer,unsigned int NumberOfSamples,float SlevelPeak,float SlevelRMS);
    void __stdcall CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,unsigned int Type,
                                                 const float  *Buffer,unsigned int NumberOfSamples);

    void __stdcall CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceCount,const void **Buffers,unsigned int NumberOfSamples,
                                                unsigned int BitsPerSample);
    void resetData();
    inline ShPtrRingPacketBuffer ddc1Buffer(){return buffer;}
private:
    ShPtrRingPacketBuffer buffer;
    const TimeReader& timeReader;

    bool isFirstBlock;
    int counterBlockPPS;
    double  currentDDCCounter;
    unsigned short currentWeekNumber;
    unsigned int currentTimeOfWeek;

    int lastBlockNumber=0;
};

#endif // WRD_CALLBACK_H
