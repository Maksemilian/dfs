#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "sync_global.h"

#include <QQueue>
#include <QObject>

/*!
 * \brief The FindChannelForShift class
 */


class Sync2D : public QObject
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
    Sync2D(const ShPtrPacketBuffer& inBuffer1,
           const ShPtrPacketBuffer& inBuffer2,
           const SyncData& data);

    ~Sync2D();
    void start();
    void stop();
  signals:
    void finished();
  private:

    double calcShiftAndShiftBuffer( VectorIpp32fc& outShiftBuffer);

    //***
    bool readPacketFromBuffer(const ShPtrPacketBuffer& buffer,
                              proto::receiver::Packet& packet,
                              quint32 sampleRate);

    //******* PREVEIOUS *********
    /*
     bool calcShiftInChannel(const ShPtrPacketBuffer& mainBuffer,
                             const ShPtrPacketBuffer& buffer,
                             quint32 sampleRate);
     static void showPacket(quint32 blockNumber, quint32 sampleRate, quint32 timeOfWeek,
                            double ddcSampleCounter, quint64 adcPeriodCounter);
     double ddcAfterPPS(double ddcSampleCounter, quint32 blockNumber, quint32 blockSize);
     void initShiftBuffer(const float* signalData,
                          quint32 size,
                          quint32 ddcDifference);*/
  private:

    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // SYNC_SHIFT_FINDER_H
