#ifndef SYNC_BLOCK_ALINEMENT_H
#define SYNC_BLOCK_ALINEMENT_H

#include "ippbase.h"

#include <QObject>
#include <memory>

using VectorIpp32fc=std::vector<Ipp32fc>;
using VectorIpp32f=std::vector<Ipp32f>;

/*!
 * \brief The BlockAlinement class
 */
class BlockEqualizer
{
public:
    //TODO blockSize лишний тк внутринние буфера должны быть
    //равными размеру shiftBuffer
    BlockEqualizer(const VectorIpp32fc&shiftBuffer,quint32 blockSize);
    ~BlockEqualizer();

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

#endif // SYNC_BLOCK_ALINEMENT_H
