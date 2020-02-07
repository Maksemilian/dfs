#include "sync_calc_delta_pps.h"

ostream& operator<<(ostream& out, const Packet& pct)
{
    out << "pct_1:" << pct.block_number()
        << " TOW:" <<  pct.time_of_week()
        << " DDC_C:" << pct.ddc_sample_counter()
        << " ACP_C:" << pct.adc_period_counter() << endl;
    return out;
}

/*! \addtogroup sync Sync
 */
///@{
double CalcDeltaPPS::findDeltaPPS()
{
//        VectorIpp32fc outShiftBuffer(_data.blockSize);
    Packet pct1 ;
    Packet pct2 ;
    double deltaPPS = -1;
    // while(deltaPPS<0){
    //WARNING В ТЕКУЩЕЙ ВЕРСИИ ПРЕДПОЛАГАЕТСЯ ЧТО
    // 1 канал стартовал раньше

    bool isPacket1Read = _channel1->readIn();

    bool isPacket2Read = _channel2->readIn();

    if(isPacket1Read && isPacket2Read)
    {
        //Packet& pct1 = _channel1->lastPacket();
        //Packet& pct2 = _channel2->lastPacket();
        _channel1->getLastPacket(pct1);
        _channel2->getLastPacket(pct2);

        if(pct1.time_of_week() !=
                pct2.time_of_week())
        {
            cout << "TOW NOT EQU:" <<
                 pct1.time_of_week() << " " << pct2.time_of_week() << endl;

            isPacket1Read = false;
            isPacket2Read = false;
            //continue;//WARNING
        }

        if(pct1.time_of_week() == pct2.time_of_week())// 1 НОМЕРА СЕКУНД ОДИНАКОВЫЕ
        {
            deltaPPS = ddcAfterPPS(pct1) - ddcAfterPPS(pct2);
        }
        else // 2 НОМЕРА СЕКУНД РАЗНЫЕ
        {
            deltaPPS = calcDeltaPPS(pct1, pct2);
        }
        cout << "SHIFT_VALUE:" << deltaPPS << endl;
        cout << "BS:" << _data.blockSize << " SR:" << _data.sampleRate << endl;
        cout << pct1 << pct2 << endl;

    }
    return deltaPPS;
    //}
    // return false;
//        return nullptr;
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
