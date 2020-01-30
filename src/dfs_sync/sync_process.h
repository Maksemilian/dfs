#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "ippbase.h"
#include <memory>
#include <QPair>
#include <QQueue>
#include <QObject>

using RingIpp32fcBuffer = RingBuffer<std::vector<Ipp32fc>>;
using RingPacketBuffer = RingBuffer<proto::receiver::Packet>;

using ShPtrIpp32fcBuffer = std::shared_ptr<RingIpp32fcBuffer>;
using ShPtrPacketBuffer = std::shared_ptr<RingPacketBuffer>;

using ShPtrPacketBufferPair = QPair<ShPtrPacketBuffer, ShPtrPacketBuffer>;

using BoolPair = QPair<bool, bool>;
/*!
 * \brief The FindChannelForShift class
 */
struct SyncData
{
    quint32 ddcFrequency;
    quint32 sampleRate;
    quint32 blockSize;
};

class SyncProcess : public QObject
{
    Q_OBJECT

    enum Channel
    {
        CHANNEL_FIRST = 0,
        CHANNEL_SECOND = 1,
        CHANNEL_SIZE = 2
    };
    static const unsigned int COUNT_SIGNAL_COMPONENT = 2;
  public:
    SyncProcess(const ShPtrPacketBuffer& outBuffer1,
                const ShPtrPacketBuffer& outBuffer2,
                const ShPtrIpp32fcBuffer& outSumDivBuffer);

    ~SyncProcess();

    void start(const ShPtrPacketBuffer& mainBuffer,
               const ShPtrPacketBuffer& buffer,
               const SyncData& data);
    void stop();
  signals:
    void finished();
  private:

    bool calcShiftInChannel(const ShPtrPacketBuffer& mainBuffer,
                            const ShPtrPacketBuffer& buffer,
                            quint32 sampleRate);

    static void showPacket(quint32 blockNumber, quint32 sampleRate, quint32 timeOfWeek,
                           double ddcSampleCounter, quint64 adcPeriodCounter);
    double ddcAfterLastPps(double ddcSampleCounter, quint32 blockNumber, quint32 blockSize);
    void initShiftBuffer(const float* signalData, quint32 size, quint32 ddcDifference);
    double calcDeltaStart();
  private:

    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // SYNC_SHIFT_FINDER_H
