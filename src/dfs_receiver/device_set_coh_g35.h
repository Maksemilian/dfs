#ifndef COH_G35_DEVICE_SET_H
#define COH_G35_DEVICE_SET_H

#include "G35DDCAPI.h"

#include <QPair>
#include <memory>
#include "ring_buffer.h"
#include "receiver.pb.h"
class Packet;
class TimeReader;
//class SignalFileWriter;
struct CohG35DeviceSetSettings{
    unsigned int attenuator;
    QPair<unsigned int,unsigned int> preselectors;
    bool preamplifier;
    bool adcEnabled;
    unsigned short threshold;
    unsigned int frequency;
    unsigned int ddcType;
    unsigned int samplesPerBuffer;
    bool powerEnabled;
};

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

    CohG35DeviceSet();
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

    bool setSettings(const CohG35DeviceSetSettings &settings);
    //*****Stram DDC1
    void startDDC1(unsigned int sampesPerBuffer,bool writeToFile=false);
    void stopDDC1();

    void reStartDdc1(unsigned int ddc1TypeIndex,unsigned int sampePerBuffer,bool writeToFile=false);

    bool setUpDeviceSet(quint32 numberDeviceSet);
    COH_G35DDC_DEVICE_SET getDeviceSetInfo();
//    std::shared_ptr<RingPacketBuffer>getDdc1Buffer(){
//        return ddc1Buffer;
//    }
    std::shared_ptr<RingBuffer<proto::receiver::Packet>>getBuffer(){
        return buffer;
    }
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

    void fillPacket(Packet &packet,DDC1StreamCallbackData &ddcStreamCallbackData,
                    double ddcSampleCounter,unsigned long long adcPeriodCounter,int counterBlockPPS);
    void showPacket(Packet &packet);

    void fillPacketT(proto::receiver::Packet &packet,DDC1StreamCallbackData &ddcStreamCallbackData,
                    double ddcSampleCounter,unsigned long long adcPeriodCounter,int counterBlockPPS);
    void showPacketT(proto::receiver::Packet &packet);

    void freeResource();
private:

    ICohG35DDCDeviceSet *deviceSet=nullptr;
//    RingPacketBuffer*ddc1Buffer=nullptr;
//    std::shared_ptr<RingPacketBuffer>ddc1Buffer;
    std::shared_ptr<RingBuffer<proto::receiver::Packet>>buffer;
    TimeReader *timeReader=nullptr;
//    SignalFileWriter *signalFileWriter=nullptr;

    bool isFirstBlock;
    int counterBlockPPS;
    double  currentDDCCounter;
    unsigned short currentWeekNumber;
    unsigned int currentTimeOfWeek;

    int lastBlockNumber=0;
};

#endif // COH_G35_DEVICE_SET_H
