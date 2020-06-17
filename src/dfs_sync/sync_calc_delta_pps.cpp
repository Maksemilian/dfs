#include "sync_calc_delta_pps.h"

/*! \addtogroup sync
 */
///@{

ostream& operator<<(ostream& out, const Packet& pct)
{
    out << "pct_1:" << pct.block_number()
        << " TOW:" <<  pct.time_of_week()
        << " DDC_C:" << pct.ddc_sample_counter()
        << " ACP_C:" << pct.adc_period_counter() << endl;
    return out;
}


double CalcDeltaPPS::findDeltaPPS()
{
    Packet pct1 ;
    Packet pct2 ;
    double deltaPPS = -1;

    bool isPacket1Read = _channel1->readIn();

    bool isPacket2Read = _channel2->readIn();

    if(isPacket1Read && isPacket2Read)
    {
        _channel1->getLastPacket(pct1);
        _channel2->getLastPacket(pct2);

        if(pct1.time_of_week() !=
                pct2.time_of_week())
        {
            cout << "TOW NOT EQU:" <<
                 pct1.time_of_week() << " " << pct2.time_of_week() << endl;

            isPacket1Read = false;
            isPacket2Read = false;
        }

        if(pct1.time_of_week() == pct2.time_of_week())// 1 НОМЕРА СЕКУНД ОДИНАКОВЫЕ
        {
            deltaPPS = ddcAfterPPS(pct1) - ddcAfterPPS(pct2);
        }
        else if(pct1.time_of_week() > pct2.time_of_week())// 2 НОМЕРА СЕКУНД РАЗНЫЕ
        {
            deltaPPS = calcDeltaPPS(pct1, pct2);
        }
        else
        {
        }
        cout << "SHIFT_VALUE:" << deltaPPS << endl;
        cout << "BS:" << _data.blockSize << " SR:" << _data.sampleRate << endl;
        cout << pct1 << pct2 << endl;

    }
    return deltaPPS;
}

double CalcDeltaPPS::calcDeltaPPS(const Packet _pct1, const Packet _pct2) const
{
    return (_pct1.time_of_week() - _pct2.time_of_week()) * _data.sampleRate
           + ddcAfterPPS(_pct1) - ddcAfterPPS(_pct2);
}

double CalcDeltaPPS::ddcAfterPPS(const Packet& pct) const
{
    return (pct.block_number() * _data.blockSize)
           - pct.ddc_sample_counter();
}

double CalcDeltaPPS::deltaStart(const Packet _pct1, const Packet _pct2)
{
    return calcDeltaPPS(_pct1, _pct2) -
           (_pct1.block_number() - _pct2.block_number()) *
           _data.blockSize;
}
///@}
