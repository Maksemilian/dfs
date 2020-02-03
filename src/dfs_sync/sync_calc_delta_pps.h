#ifndef SYNC_CALC_DALTA_PPS_H
#define SYNC_CALC_DALTA_PPS_H

#include "sync_global.h"

#include <iostream>
#include <qglobal.h>
using namespace  std;


/*!
 * \brief SyncProcess::calcShiftInChannelTEST
 * \param buffer1 - буфер опорного канала
 * \param buffer2 - буфер канала синхронизации
 * \param sampleRate
 * \return
 */
/*!
 * \brief The CalcDeltaPPS class
 */
class CalcDeltaPPS
{
  public:
    CalcDeltaPPS(proto::receiver::Packet& packet1,
                 proto::receiver::Packet& packet2,
                 unsigned int sampleRate,
                 unsigned int blockSize = 8192):
        _pct1(packet1),
        _pct2(packet2),
        _sampleRate(sampleRate),
        _blockSize(blockSize)
    {
        Q_ASSERT_X(_pct1.sample_rate() == _sampleRate &&
                   _pct2.sample_rate() == _sampleRate &&
                   _pct1.block_size() == _blockSize &&
                   _pct2.block_size() == _blockSize,
                   "CalcDeltaPPS::Constructor",
                   "bad input data");

    }

    double deltaPPS() const
    {
        return (_pct1.time_of_week() - _pct2.time_of_week()) * _sampleRate
               + ddcAfterPPS1() - ddcAfterPPS2();
    }

    double deltaStart() const
    {
        return deltaPPS() -
               (_pct1.block_number() - _pct2.block_number()) *
               _blockSize;
    }
    //double
    double ddcAfterPPS1() const
    {
        return (_pct1.block_number() * _blockSize)
               - _pct1.ddc_sample_counter();
    }

    double ddcAfterPPS2() const
    {
        return (_pct2.block_number() * _blockSize)
               - _pct2.ddc_sample_counter();
    }

    VectorIpp32fc initShiftBuffer()
    {
        const float* signalData = _pct2.mutable_sample()->data();
        quint32 shift = static_cast<quint32>(deltaPPS());
        VectorIpp32fc _shiftBuffer(_blockSize);
        if(shift < _blockSize)
        {
            for(quint32 i = 0; i < shift; ++i)
            {
                _shiftBuffer[i].re = signalData[_blockSize - (shift) + i * 2];
                _shiftBuffer[i].im = signalData[(_blockSize - (shift) + i * 2) + 1];
            }
        }
        return _shiftBuffer;
    }

    friend ostream& operator<<(ostream& out, const CalcDeltaPPS& c);
  private:
    proto::receiver::Packet& _pct1;
    proto::receiver::Packet& _pct2;
    unsigned int _sampleRate = 0;
    unsigned int _blockSize = 0;

};

ostream& operator<<(ostream& out, const CalcDeltaPPS& c)
{
    const proto::receiver::Packet& pct1 = c._pct1;
    const proto::receiver::Packet& pct2 = c._pct2;
    out << "BS:" << c._blockSize << " SR:" << c._sampleRate << endl;
    out << "pct_1:" << pct1.block_number()
        << " TOW:" <<  pct1.time_of_week()
        << " DDC_C:" << pct1.ddc_sample_counter()
        << " ACP_C:" << pct1.adc_period_counter() << endl;

    out << "pct_2:" << pct2.block_number()
        << " TOW:" <<  pct2.time_of_week()
        << " DDC_C:" << pct2.ddc_sample_counter()
        << " ACP_C:" << pct2.adc_period_counter() ;

    return out;
}

#endif // SYNC_CALC_DALTA_PPS_H
