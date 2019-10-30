#ifndef SYNC_PAIR_CHANNEL_H
#define SYNC_PAIR_CHANNEL_H


#include "ring_buffer.h"
#include "ippbase.h"
#include <memory>
#include <QPair>

class RingBuffer;


using ShPtrBuffer=std::shared_ptr<RingBuffer>;
using ShPtrBufferT=std::shared_ptr<RingBufferT<std::vector<Ipp32fc>>>;
using ShPtrBufferPair=QPair<ShPtrBuffer,ShPtrBuffer>;
using VectorIpp32fc=std::vector<Ipp32fc>;
using VectorIpp32f=std::vector<Ipp32f>;
using BoolPair=QPair<bool,bool>;

/*!
 * \brief The BlockAlinement class
 */
class BlockAlinement
{
public:
    BlockAlinement(const std::vector<Ipp32fc>&shiftBuffer,quint32 blockSize);
    ~BlockAlinement();

    void equate(Ipp32fc *signal,quint32 size,
                double shift,
                quint32 ddcFrequency,
                quint32 sampleRate,
                double deltaStart)const;
private:
    void initFftBuffers(int FFTOrder);
    int  calcFftOrder(quint32 number);

    void shiftWhole(Ipp32fc *signal,quint32 blockSize,quint32 wholeShift)const;
    void shiftFruction(Ipp32fc *signal,quint32 size,double phaseAngle)const;
    void shiftTest(Ipp32fc *signal,quint32 blockSize,double teta)const;

    void swapHalfPhaseValues(Ipp32f*phaseData,quint32 size)const;
    void incrementPhaseValuesOnAngle(Ipp32f *phaseData,quint32 size,float phaseAngle)const;
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};

/*!
 * \brief The FindChannelForShift class
 */
class ShiftFinder
{
    enum Channel{
        CHANNEL_FIRST=0,
        CHANNEL_SECOND=1,
        CHANNEL_SIZE=2
    };
    static const unsigned int COUNT_SIGNAL_COMPONENT=2;
public:
    ShiftFinder(quint32 sampleRate,quint32 blockSize);
    ~ShiftFinder();
    bool calcShiftInChannel(const ShPtrBufferPair receiverStationClientPair);
    int getChannelIndex();

    const VectorIpp32fc& getShiftBuffer();

    double getShiftValue();
    double getDeltaStart();

    void resetSampleRatePair();

    static void showPacket(quint32 blockNumber,quint32 sampleRate,quint32 timeOfWeek,
                           double ddcSampleCounter,quint64 adcPeriodCounter);
private:
    double ddcAfterLastPps(double ddcSampleCounter,quint32 blockNumber,quint32 blockSize);
    void initShiftBuffer(const float *signalData,quint32 size,quint32 ddcDifference);
    double calcDeltaStart();
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};

/*!
 * \brief The SyncPairChannel class
 *Сначала идет сдвиг пакета на количество отсчетов равное ddcDifference
 * а затем полученный из полученного сигнала берется фазовая состовляющая
 * и каждый элемент фазы увеличивается на значение phaseAngle
 */
class SyncPairChannel
{
    enum Channel{
        CHANNEL_FIRST=0,
        CHANNEL_SECOND=1,
        CHANNEL_SIZE=2
    };
    static const unsigned int COUNT_SIGNAL_COMPONENT=2;
    static const int INDEX=0;
public:
    SyncPairChannel();
    ~SyncPairChannel();

    ShPtrBuffer syncBuffer1();
    ShPtrBuffer syncBuffer2();
    ShPtrBufferT sumDivMethod();

    void start(const ShPtrBufferPair receiverStationClientPair,
               quint32 ddcFrequency,quint32 samplerate,quint32 blockSize);
    void stop();

    void enableWholeShift();
    void disableWholeShift();

    void enableFructionShift();
    void disabledFructionShift();
private:
    void sync(const ShPtrBufferPair receiverStationClientPair,
              quint32 ddcFrequency,quint32 samplerate,quint32 blockSize);

    //    void sync(const std::shared_ptr<RingBuffer> buffer1,
    //              const std::shared_ptr<RingBuffer> buffer2,
    //              quint32 ddcFrequency,quint32 samplerate,quint32 blockSize);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // SYNC_PAIR_CHANNEL_H
