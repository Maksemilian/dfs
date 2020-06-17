#ifndef SYNC_CALC_DALTA_PPS_H
#define SYNC_CALC_DALTA_PPS_H

#include "radio_channel.h"

#include <iostream>
#include <qglobal.h>

using namespace  std;
using namespace proto::receiver;


/*! \addtogroup sync
 */
///@{

/*!
 * \brief Класс для определения межканального смещения по шкале времени,
 *  измеряемого количеством отсчетов DDC. Смещение определяется
 *  разностью отсчетов после приема импульса PPS
 *  с одним и тем же номером в 1-ом и 2-ом каналах.
 *  Количество отсчетов после приема импульса PPS
 *  в каждом канале вычисляется по формуле:
 *  AfterPPS = NumBlock × SizeBlock – CntDDC.
 *
 * \todo Убрать ChannelData
 */
class CalcDeltaPPS
{
  public:
    CalcDeltaPPS(const ShPtrRadioChannel& channel1,
                 const ShPtrRadioChannel& channel2,
                 const ChannelData& data)
        : _channel1(channel1), _channel2(channel2), _data(data)    {   }
    /*!
     * \brief Метод нахождения разности отсчетов DDC между двумя каналами
     * после приема последнего импульса PPS.
     * Из обоих каналов последовательно считываются данные до тех пор пока
     * не будут полученны данные в обоих каналах.
     * Если блоки считанных данных принадлежат к одной секунде происходит
     * расчет количества ddc отсчетов после последнего импульса PPS для
     * каждого канала. Затем определяется разность полученных значений ,
     * которая является задержкой канала channel2 от канала channel1.
     *
     * \return смещение канала channel2 от channel1 ,
     * выраженная разницой между ddc отсчетами двух каналов полученыыми
     * после последнего импульса PPS.
     */
    double findDeltaPPS();
  private:
    double calcDeltaPPS(const Packet _pct1, const Packet _pct2) const;

    double ddcAfterPPS(const Packet& pct) const;
    double deltaStart(const Packet _pct1, const Packet _pct2) ;
  private:
    ShPtrRadioChannel _channel1;
    ShPtrRadioChannel _channel2;
    ChannelData _data;
};
///@}

#endif // SYNC_CALC_DALTA_PPS_H
