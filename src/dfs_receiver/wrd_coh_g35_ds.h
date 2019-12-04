#ifndef COH_G35_DEVICE_SET_H
#define COH_G35_DEVICE_SET_H

#include "coh_g35_ds_settings.h"

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "G35DDCAPI.h"

#include <QPair>
#include <memory>

class TimeReader;
using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class CohG35DeviceSet :public ICohG35DDCDeviceSetCallback
{
public:
    static const int TIMEOUT=2000;
    static const int COUNT_SIGNAL_COMPONENT=2;

    struct DDC1StreamCallbackData{
        ICohG35DDCDeviceSet *DeviceSet;
        unsigned int DeviceCount;
        const void **Buffers;
        unsigned int NumberOfSamples;
        unsigned int BitsPerSample;
    };

    CohG35DeviceSet( ICohG35DDCDeviceSet *_deviceSet,const TimeReader &timeReader);
    virtual ~CohG35DeviceSet();
public:
    bool setPower(bool state);
    bool setAttenuator(unsigned int attenuationLevel);
    bool setPreselectors(unsigned int lowFrequency,unsigned int highFrequency);
    bool setPreamplifierEnabled(bool state);
    bool setDDC1Frequency(unsigned int ddc1Frequency);
    bool setAdcNoiceBlankerEnabled(bool state);
    bool setAdcNoiceBlankerThreshold(unsigned short threshold);
    bool setDDC1Type(quint32 type);

    bool setSettings(const DeviceSetSettings &settings);
    //*****Stram DDC1
    bool startDDC1(unsigned int sampesPerBuffer);
    bool stopDDC1();

    void reStartDdc1(unsigned int ddc1TypeIndex,unsigned int sampePerBuffer);

    bool setShiftPhaseDDC1(unsigned int deviceIndex,double phaseShift);
    inline ShPtrRingPacketBuffer getBuffer(){
        return buffer;
    }

    COH_G35DDC_DEVICE_SET getDeviceSetInfo();
    QString getDeviceSetName();
private:
    void __stdcall CohG35DDC_IFCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,const short *Buffer,unsigned int NumberOfSamples,
                                        WORD MaxADCAmplitude,unsigned int ADCSamplingRate);
    void __stdcall CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,const float *Buffer,unsigned int NumberOfSamples);
    void __stdcall CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,
                                                            const float  *Buffer,unsigned int NumberOfSamples,float SlevelPeak,float SlevelRMS);
    void __stdcall CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceIndex,unsigned int Type,
                                                 const float  *Buffer,unsigned int NumberOfSamples);

    void __stdcall CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet *DeviceSet,unsigned int DeviceCount,const void **Buffers,unsigned int NumberOfSamples,
                                                unsigned int BitsPerSample);

private:
    void resetData();

    void fillPacket(proto::receiver::Packet &packet,DDC1StreamCallbackData &ddcStreamCallbackData,
                    double ddcSampleCounter,unsigned long long adcPeriodCounter,int counterBlockPPS);
    void showPacket(proto::receiver::Packet &packet);

    void freeResource();
private:

    ICohG35DDCDeviceSet *_deviceSet=nullptr;
    ShPtrRingPacketBuffer buffer;
    TimeReader& timeReader;

    bool isFirstBlock;
    int counterBlockPPS;
    double  currentDDCCounter;
    unsigned short currentWeekNumber;
    unsigned int currentTimeOfWeek;

    int lastBlockNumber=0;
};

#endif // COH_G35_DEVICE_SET_H
