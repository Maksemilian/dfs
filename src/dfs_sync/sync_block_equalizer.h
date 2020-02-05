#ifndef SYNC_BLOCK_ALINEMENT_H
#define SYNC_BLOCK_ALINEMENT_H

#include "sync_global.h"
#include "sync_radio_channel.h"

#include <QObject>
/*!
 * \brief The BlockAlinement class
 */
class BlockEqualizer
{
  public:
    //TODO blockSize лишний тк внутринние буфера должны быть
    //равными размеру shiftBuffer
    //BlockEqualizer(const VectorIpp32fc& shiftBuffer, quint32 blockSize);
//    BlockEqualizer(const ShPtrRadioChannel& channel,
//                   const VectorIpp32fc& shiftBuffer,
//                   const SyncData& data,
//                   quint32 shift);

    BlockEqualizer(const VectorIpp32fc& shiftBuffer,
                   const SyncData& data, quint32 shift);
    ~BlockEqualizer();
    //TODO double deltaStart=1
    void shiftChannel(ShPtrRadioChannel& channel);
    void equateT(const proto::receiver::Packet& pct1, double deltaStart = 1)const;
  private:
    void equate(Ipp32fc* signal, quint32 size, double deltaStart = 1)const;
    void initFftBuffers(int FFTOrder);
    int  calcFftOrder(quint32 number);

    void shiftWhole(Ipp32fc* signal, quint32 blockSize, quint32 wholeShift)const;
    void shiftFruction(Ipp32fc* signal, quint32 size, double phaseAngle)const;
    void shiftTest(Ipp32fc* signal, quint32 blockSize, double teta)const;

    void swapHalfPhaseValues(Ipp32f* phaseData, quint32 size)const;
    void incrementPhaseValuesOnAngle(Ipp32f* phaseData, quint32 size, float phaseAngle)const;
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};

#endif // SYNC_BLOCK_ALINEMENT_H
