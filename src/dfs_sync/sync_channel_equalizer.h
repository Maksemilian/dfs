#ifndef SYNC_BLOCK_ALINEMENT_H
#define SYNC_BLOCK_ALINEMENT_H

#include "radio_channel.h"
#include <QObject>
#include "ippbase.h"

/*! \addtogroup sync Sync
 */
///@{

/*!
 * \brief Класс сдвигает данные канала на количество равное
 * целой части deltaPPS
 *
 * Для того чтобы сдвигать сдвигать данные канала необходимо
 * единожды проинициализировать буфер сдвига и только после
 * этого сдвигать канал
 */
class ChannelEqualizer
{
  public:
    ChannelEqualizer(const ShPtrRadioChannel& channel, double deltaPPS);

    ~ChannelEqualizer() ;
    //TODO double deltaStart=1
    void shiftChannel();
    void initShiftBuffer();
  private:
    void equatePacket(const proto::receiver::Packet& pct1, double deltaStart = 1)const;
    void equate(Ipp32fc* signal, quint32 size, double deltaStart = 1)const;
    void shiftWhole(Ipp32fc* signal, quint32 blockSize, quint32 wholeShift)const;
    void shiftFruction(Ipp32fc* signal, quint32 size, double phaseAngle)const;

    void initFftBuffers(int FFTOrder);
    int  calcFftOrder(quint32 number);

    void shiftTest(Ipp32fc* signal, quint32 blockSize, double teta)const;
    void swapHalfPhaseValues(Ipp32f* phaseData, quint32 size)const;
    void incrementPhaseValuesOnAngle(Ipp32f* phaseData, quint32 size, float phaseAngle)const;
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
///@}
#endif // SYNC_BLOCK_ALINEMENT_H
