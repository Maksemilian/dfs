#ifndef SYNC_PAIR_CHANNEL_H
#define SYNC_PAIR_CHANNEL_H

#include "sync_process.h"

/*!
 * \brief The SyncPairChannel class
 *Сначала идет сдвиг пакета на количество отсчетов равное ddcDifference
 * а затем полученный из полученного сигнала берется фазовая состовляющая
 * и каждый элемент фазы увеличивается на значение phaseAngle
 */
class SyncController
{
  public:
    SyncController();
    ~SyncController();

    ShPtrPacketBuffer syncBuffer1();
    ShPtrPacketBuffer syncBuffer2();
    ShPtrIpp32fcBuffer sumDivMethodBuffer();

    void start(const ShPtrPacketBufferPair inPacketBuffers,
               quint32 ddcFrequency, quint32 samplerate, quint32 blockSize);
    void stop();
  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

#endif // SYNC_PAIR_CHANNEL_H
