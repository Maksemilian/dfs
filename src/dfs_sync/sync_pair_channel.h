#ifndef SYNC_PAIR_CHANNEL_H
#define SYNC_PAIR_CHANNEL_H

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

    ShPtrPacketBuffer syncBuffer1();
    ShPtrPacketBuffer syncBuffer2();
    ShPtrIpp32fcBuffer sumDivMethod();

    void start(const ShPtrPacketBufferPair receiverStationClientPair,
               quint32 ddcFrequency,quint32 samplerate,quint32 blockSize);
    void stop();

    void enableWholeShift();
    void disableWholeShift();

    void enableFructionShift();
    void disabledFructionShift();
private:
    void sync(const ShPtrPacketBufferPair receiverStationClientPair,
              quint32 ddcFrequency,quint32 samplerate,quint32 blockSize);
private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // SYNC_PAIR_CHANNEL_H
