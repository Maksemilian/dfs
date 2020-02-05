#ifndef SYNC_CALC_DALTA_PPS_H
#define SYNC_CALC_DALTA_PPS_H

#include "sync_global.h"
#include "radio_channel.h"
#include "sync_channel_equalizer.h"

#include <iostream>
#include <qglobal.h>

using namespace  std;
using namespace proto::receiver;
/*!
 * \brief SyncProcess::calcShiftInChannelTEST
 * \param channel1 - опорный канала
 * \param channel2 -  синхронизуемый канал
 * \param data - SyncData
 * \return
 */
/*!
 * \brief The CalcDeltaPPS class
 */
ostream& operator<<(ostream& out, const Packet& pct)
{
    out << "pct_1:" << pct.block_number()
        << " TOW:" <<  pct.time_of_week()
        << " DDC_C:" << pct.ddc_sample_counter()
        << " ACP_C:" << pct.adc_period_counter() << endl;
    return out;
}

class CalcDeltaPPS
{
  public:
    CalcDeltaPPS(const ShPtrRadioChannel& channel1,
                 const ShPtrRadioChannel& channel2,
                 const ChannelData& data)
        : _channel1(channel1), _channel2(channel2), _data(data)    {   }

    std::unique_ptr<ChannelEqualizer> getBlockEqualizer()
    {
        VectorIpp32fc outShiftBuffer(_data.blockSize);
        Packet pct1 ;
        Packet pct2 ;
        double deltaPPS = -1;
        // while(true){
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
            cout << "BS:" << _data.blockSize << " SR:" << _data.sampleRate << endl;
            cout << pct1 << pct2 << endl;
            outShiftBuffer = initShiftBuffer(pct2, static_cast<quint32>(deltaPPS));
//            _channel2->setBlockEqulizer(new BlockEqualizer(outShiftBuffer,
//                                        _data,
//                                        static_cast<quint32>(deltaPPS)));
            cout << "SHIFT_VALUE:" << deltaPPS << endl;
            //return true;
            return std::make_unique<ChannelEqualizer>(outShiftBuffer,
                    _data,
                    static_cast<quint32>(deltaPPS));
        }
        //}
        // return false;
        return nullptr;
    }
  private:
    double calcDeltaPPS(const Packet _pct1, const Packet _pct2) const
    {
        return (_pct1.time_of_week() - _pct2.time_of_week()) * _data.sampleRate
               + ddcAfterPPS(_pct1) - ddcAfterPPS(_pct2);
    }
    double ddcAfterPPS(const Packet& pct) const
    {
        return (pct.block_number() * _data.blockSize)
               - pct.ddc_sample_counter();
    }

    double deltaStart(const Packet _pct1, const Packet _pct2) const
    {
        return calcDeltaPPS(_pct1, _pct2) -
               (_pct1.block_number() - _pct2.block_number()) *
               _data.blockSize;
    }

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

    //friend ostream& operator<<(ostream& out, const Packet& c);
  private:
    ShPtrRadioChannel _channel1;
    ShPtrRadioChannel _channel2;
    ChannelData _data;
};


#endif // SYNC_CALC_DALTA_PPS_H
