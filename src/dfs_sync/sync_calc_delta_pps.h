#ifndef SYNC_CALC_DALTA_PPS_H
#define SYNC_CALC_DALTA_PPS_H

#include "radio_channel.h"

#include <iostream>
#include <qglobal.h>

using namespace  std;
using namespace proto::receiver;


/*! \addtogroup sync Sync
 */
///@{

/*!
 * \brief Класс определения отставания канала channel2
 *  от опорного канала channel1
 *
 * Класс определяет разницу канала с помощью deltaPPS
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
     * \brief Метод нахождения разности между двумя каналами
     *
     * Из обоих каналов последовательно считываются данные до тех пор пока
     * не будут полученны данные в обоих каналах.
     * Если блоки считанных данных принадлежат к одной секунде происходит
     * расчет количества ddc отсчетов после последнего импульса PPS для
     * каждого канала. Затем определяется разность полученных значений ,
     * которая является задержкой канала channel2 от канала channel1.
     *
     * \return deltaPPS отставание канала channel2 от channel1 ,
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
/// //            outShiftBuffer = initShiftBuffer(pct2,
//                                             static_cast<quint32>(deltaPPS));
//            _channel2->setBlockEqulizer(new BlockEqualizer(outShiftBuffer,
//                                        _data,
//                                        static_cast<quint32>(deltaPPS)));
//return true;
//            return std::make_unique<ChannelEqualizer>(outShiftBuffer,
//                    _data,
//                    static_cast<quint32>(deltaPPS));
/*
    VectorIpp32fc initShiftBuffer(Packet& _pct2, quint32 shift)
    {
        const float* signalData = _pct2.mutable_sample()->data();
        VectorIpp32fc _shiftBuffer(_data.blockSize);
        if(shift < _data.blockSize)
        {
            for(quint32 i = 0; i < shift; ++i)
            {
                _shiftBuffer[i].re = signalData[_data.blockSize - (shift) + i * 2];
                _shiftBuffer[i].im = signalData[(_data.blockSize - (shift) + i * 2) + 1];
            }
        }
        return _shiftBuffer;
    }
*/
//friend ostream& operator<<(ostream& out, const Packet& c);
#endif // SYNC_CALC_DALTA_PPS_H
