#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "sync_base.h"


#include "ring_buffer.h"
#include "receiver.pb.h"

#include <memory>
#include <QPair>
#include <QQueue>

using RingIpp32fcBuffer=RingBuffer<std::vector<Ipp32fc>>;
using RingPacketBuffer=RingBuffer<proto::receiver::Packet>;

using ShPtrIpp32fcBuffer=std::shared_ptr<RingIpp32fcBuffer>;
using ShPtrPacketBuffer=std::shared_ptr<RingPacketBuffer>;

using ShPtrPacketBufferPair=QPair<ShPtrPacketBuffer,ShPtrPacketBuffer>;

using BoolPair=QPair<bool,bool>;
/*!
 * \brief The FindChannelForShift class
 */
class ShiftFinder
{
    enum Channel
    {
        CHANNEL_FIRST=0,
        CHANNEL_SECOND=1,
        CHANNEL_SIZE=2
    };
    static const unsigned int COUNT_SIGNAL_COMPONENT=2;
public:
    ShiftFinder();
    ShiftFinder(quint32 sampleRate,quint32 blockSize);
    ~ShiftFinder();
    void sync(const ShPtrPacketBufferPair buffers,
                               quint32 ddcFrequency,
                               quint32 sampleRate,
                               quint32 blockSize);
    int getChannelIndex();
    void start();
    void stop();
    const VectorIpp32fc& getShiftBuffer();

    double getShiftValue();
    double getDeltaStart();

    void setBuffer1(const ShPtrPacketBuffer buffer1);
    void setBuffer2(const ShPtrPacketBuffer buffer2);
    void setSumDivBuffer(const ShPtrIpp32fcBuffer sumDivMethod);

    ShPtrPacketBuffer syncBuffer1();

    ShPtrPacketBuffer syncBuffer2();

    ShPtrIpp32fcBuffer sumDivMethod();

    bool calcShiftInChannel(const ShPtrPacketBufferPair receiverStationClientPair,
                            quint32 sampleRate);
private:

    static void showPacket(quint32 blockNumber,quint32 sampleRate,quint32 timeOfWeek,
                           double ddcSampleCounter,quint64 adcPeriodCounter);
    double ddcAfterLastPps(double ddcSampleCounter,quint32 blockNumber,quint32 blockSize);
    void initShiftBuffer(const float *signalData,quint32 size,quint32 ddcDifference);
    double calcDeltaStart();
private:

    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // SYNC_SHIFT_FINDER_H
