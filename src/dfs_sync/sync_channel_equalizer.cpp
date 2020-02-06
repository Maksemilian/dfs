#include "sync_channel_equalizer.h"

#include "ipp.h"
#include "ippvm.h"

#include <math.h>
#include <QDebug>

//NOTE - НАХОДИТЬСЯ В utility
using DeleterTypeIpp8u = std::function<void(Ipp8u*)>;

struct ChannelEqualizer::Impl
{
    /*
    Impl(const std::vector<Ipp32fc>& shiftBuffer,
         const  ChannelData data,
         quint32 shift):
        dstFftFwd(VectorIpp32fc(data.blockSize)),
        dstMagn(VectorIpp32f(data.blockSize)),
        dstPhase(VectorIpp32f(data.blockSize)),
        dstCart32(VectorIpp32fc(data.blockSize)),
        dstFinalRez(VectorIpp32fc(data.blockSize)),
        _shiftBuffer(shiftBuffer),
        data(data)
    //        shift(shift)
    {    }*/
    Impl(const ShPtrRadioChannel& channel,
         double deltaPPS)
    {
        _data = channel->getChannelData();
        dstFftFwd = (VectorIpp32fc(_data.blockSize));
        dstMagn = (VectorIpp32f(_data.blockSize));
        dstPhase = (VectorIpp32f(_data.blockSize));
        dstCart32 = (VectorIpp32fc(_data.blockSize));
        dstFinalRez = (VectorIpp32fc(_data.blockSize));
        _channel = channel;
        _deltaPPS = deltaPPS;
    }


    DeleterTypeIpp8u deleterUniquePtrForIpp8u = [](Ipp8u* v)
    {
        ippFree(v);
    };

    std::unique_ptr<Ipp8u, DeleterTypeIpp8u> pMemInit;
    std::unique_ptr<Ipp8u, DeleterTypeIpp8u> pMemBuffer;
    std::unique_ptr<Ipp8u, DeleterTypeIpp8u> pMemSpec;

    VectorIpp32fc dstFftFwd;
    VectorIpp32f dstMagn;
    VectorIpp32f dstPhase;
    VectorIpp32fc dstCart32;
    VectorIpp32fc dstFinalRez;

    ShPtrRadioChannel _channel;
    double _deltaPPS;
    VectorIpp32fc _shiftBuffer;
    ChannelData _data;
//    quint32 shift;
};
/*
BlockEqualizer::BlockEqualizer(const VectorIpp32fc& shiftBuffer, quint32 blockSize):
    d(std::make_unique<Impl>(shiftBuffer, blockSize))
{
    initFftBuffers(calcFftOrder(blockSize));
}
*/
//BlockEqualizer::BlockEqualizer(const ShPtrRadioChannel& channel,
//                               const VectorIpp32fc& shiftBuffer,
//                               const SyncData& data, quint32 shift):
//    d(std::make_unique<Impl>(channel, shiftBuffer, data, shift)) {}
ChannelEqualizer::ChannelEqualizer(const ShPtrRadioChannel& channel,
                                   double deltaPPS)
    : d(std::make_unique<Impl>(channel, deltaPPS))
{

}
/*
ChannelEqualizer::ChannelEqualizer(const VectorIpp32fc& shiftBuffer,
                                   const ChannelData& data, quint32 shift):
    d(std::make_unique<Impl>(shiftBuffer, data, shift)) {}
*/
ChannelEqualizer::~ChannelEqualizer() = default;

void ChannelEqualizer::initShiftBuffer()
{
    quint32 shift = static_cast<quint32>(d->_deltaPPS);
    proto::receiver::Packet _pct2;
    if(d->_channel->getLastPacket(_pct2))
    {

        const float* signalData = _pct2.mutable_sample()->data();
        VectorIpp32fc _shiftBuffer(d->_data.blockSize);
        if(shift < d->_data.blockSize)
        {
            for(quint32 i = 0; i < shift; ++i)
            {
                _shiftBuffer[i].re = signalData[d->_data.blockSize - (shift) + i * 2];
                _shiftBuffer[i].im = signalData[(d->_data.blockSize - (shift) + i * 2) + 1];
            }
        }
    }
}

void ChannelEqualizer::initFftBuffers(int FFTOrder)
{
    qDebug() << "BlockAlinement::init FFTOrder" << FFTOrder;
    Q_ASSERT_X(FFTOrder > 0, "BlockAlinement::initFftBuffers", "fft order below 0");

    int sizeSpec = 0;
    int sizeInit = 0;
    int sizeBuffer = 0;

    int flag = IPP_FFT_DIV_FWD_BY_N;
    ippsFFTGetSize_C_32fc(FFTOrder, flag, ippAlgHintNone,
                          &sizeSpec, &sizeInit, &sizeBuffer);

    std::unique_ptr<Ipp8u, DeleterTypeIpp8u>pMemSpecLocal(static_cast<Ipp8u*>(ippMalloc(sizeSpec)),
            d->deleterUniquePtrForIpp8u);
    d->pMemSpec = std::move(pMemSpecLocal);
    if (sizeInit > 0)
    {
        std::unique_ptr<Ipp8u, DeleterTypeIpp8u>pMemInitLocal(static_cast<Ipp8u*>(ippMalloc(sizeInit)),
                d->deleterUniquePtrForIpp8u);
        d->pMemInit = std::move(pMemInitLocal);
    }
    if (sizeBuffer > 0)
    {
        std::unique_ptr<Ipp8u, DeleterTypeIpp8u> pMemBufferLocal(static_cast<Ipp8u*>(ippMalloc(sizeBuffer)),
                d->deleterUniquePtrForIpp8u);
        d->pMemBuffer = std::move(pMemBufferLocal);
    }

    ippsFFTInit_C_32fc(nullptr, FFTOrder, flag, ippAlgHintNone, d->pMemSpec.get(), d->pMemInit.get());
    qDebug() << "BlockAlinement::init End";
}

void ChannelEqualizer::shiftChannel()
{
    double deltaStart = -1;
    proto::receiver::Packet pct;
    quint32 shift = static_cast<quint32>(d->_deltaPPS);
    if(d->_channel->getLastPacket(pct))
    {
        //        equatePacket(pct);
        Q_ASSERT_X(pct.block_size() == d->_data.blockSize,
                   " BlockEqualizer::equate", "out_of_range");

        Ipp32fc* blockData = reinterpret_cast<Ipp32fc*>
                             (const_cast<float*>
                              (pct.sample().data()));

        quint32 shiftW = static_cast<quint32>(shift);
        double shiftF = shift - shiftW;
        //    qDebug()<<"SHIFT"<<shift<<"W:"<<shiftW<<"F:"<<shiftF<<blockSize<<blockSize/2;
        shiftWhole(blockData, d->_data.blockSize, shiftW);

        //    qDebug()<<"SHIFT WHOLE END";

        shiftFruction(blockData, d->_data.blockSize, shiftF);
        //    qDebug()<<"SHIFT FRUCTION END";
        double teta = (d->_data.ddcFrequency / d->_data.sampleRate) * 2 * IPP_PI * deltaStart;

        shiftTest(blockData, d->_data.blockSize, teta);
        qDebug() << "*******TETA:" << teta << d->_data.ddcFrequency << d->_data.sampleRate << deltaStart;
        //qDebug()<<"SHIFT FRUCTIOM END";
        d->_channel->outBuffer()->push(pct);
    }
    else
    {
        qDebug() << "BlockEqualizer::applyShift failed";
    }
}

void ChannelEqualizer::equatePacket(const proto::receiver::Packet& pct, double deltaStart) const
{
    Q_ASSERT_X(pct.block_size() == d->_data.blockSize,
               " BlockEqualizer::equate", "out_of_range");
    quint32 shift = static_cast<quint32>(d->_deltaPPS);
    Ipp32fc* blockData = reinterpret_cast<Ipp32fc*>
                         (const_cast<float*>
                          (pct.sample().data()));

    quint32 shiftW = static_cast<quint32>(shift);
    double shiftF = shift - shiftW;
    //    qDebug()<<"SHIFT"<<shift<<"W:"<<shiftW<<"F:"<<shiftF<<blockSize<<blockSize/2;
    shiftWhole(blockData, d->_data.blockSize, shiftW);

    //    qDebug()<<"SHIFT WHOLE END";

    shiftFruction(blockData, d->_data.blockSize, shiftF);
    //    qDebug()<<"SHIFT FRUCTION END";
    double teta = (d->_data.ddcFrequency / d->_data.sampleRate) * 2 * IPP_PI * deltaStart;

    shiftTest(blockData, d->_data.blockSize, teta);
    qDebug() << "*******TETA:" << teta << d->_data.ddcFrequency << d->_data.sampleRate << deltaStart;
    //qDebug()<<"SHIFT FRUCTIOM END";
}


void ChannelEqualizer::equate(Ipp32fc* blockData, quint32 blockSize,
                              double deltaStart) const
{
    quint32 shift = static_cast<quint32>(d->_deltaPPS);
    quint32 shiftW = static_cast<quint32>(shift);
    double shiftF = shift - shiftW;
    //    qDebug()<<"SHIFT"<<shift<<"W:"<<shiftW<<"F:"<<shiftF<<blockSize<<blockSize/2;
    shiftWhole(blockData, blockSize, shiftW);

    //    qDebug()<<"SHIFT WHOLE END";

    shiftFruction(blockData, blockSize, shiftF);
    //    qDebug()<<"SHIFT FRUCTION END";
    double teta = (d->_data.ddcFrequency / d->_data.sampleRate) * 2 * IPP_PI * deltaStart;

    shiftTest(blockData, blockSize, teta);
    qDebug() << "*******TETA:" << teta << d->_data.ddcFrequency << d->_data.sampleRate << deltaStart;
    //qDebug()<<"SHIFT FRUCTIOM END";
}

void ChannelEqualizer::shiftWhole(Ipp32fc* blockData, quint32 blockSize, quint32 shift) const
{
    Q_ASSERT_X(shift < blockSize, "SignalSync::shift64", "shift > blockSize");

    std::vector<Ipp32fc> bufArray(blockSize);

    for(quint32 i = 0; i < shift; i++)
    {
        bufArray[i] = d->_shiftBuffer[i];
        d->_shiftBuffer[i] = blockData[blockSize - shift + i];
    }

    for(quint32 i = 0; i < (blockSize - shift); i++)
    {
        bufArray[i + shift] = blockData[i];
    }

    for(quint32 i = 0; i < blockSize; i++)
    {
        blockData[i] = bufArray[i];
    }
}

void ChannelEqualizer::shiftFruction(Ipp32fc* blockData, quint32 dataSize, double phaseAngle) const
{
    IppsFFTSpec_C_32fc* pSpec = reinterpret_cast<IppsFFTSpec_C_32fc*>(d->pMemSpec.get());
    //1---FFT FWD
    ippsFFTFwd_CToC_32fc(blockData, d->dstFftFwd.data(), pSpec, d->pMemBuffer.get());
    //2---CART TO POLAR
    ippsCartToPolar_32fc(d->dstFftFwd.data(), d->dstMagn.data(), d->dstPhase.data(), static_cast<int>(dataSize));
    //3--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.data(), dataSize);
    //4--SHIFT INCREMENT DST POLAR ON ADC
    incrementPhaseValuesOnAngle(d->dstPhase.data(), dataSize, static_cast<float>(phaseAngle));
    //5--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.data(), dataSize);
    //6--POLAR TO CART
    ippsPolarToCart_32fc(d->dstMagn.data(), d->dstPhase.data(), d->dstCart32.data(), static_cast<int>(dataSize));
    //7--FFT INV
    ippsFFTInv_CToC_32fc(d->dstCart32.data(), blockData, pSpec, d->pMemBuffer.get());
}

void ChannelEqualizer::shiftTest(Ipp32fc* blockData, quint32 blockSize, double teta) const
{
    Ipp32fc val;
    //    val.re=static_cast<float>(cos(teta));
    //    val.im=static_cast<float>(sin(teta));
    val.re = static_cast<float>(cos(fmod(teta, (2 * IPP_PI))));
    val.im = static_cast<float>(sin(fmod(teta, (2 * IPP_PI))));
    qDebug() << "VAL" << val.re << val.im << static_cast<quint64>(teta) << fmod(teta, (2 * IPP_PI));
    ippsMulC_32fc(blockData, val, blockData, static_cast<int>(blockSize));
    //    ippsMulC_32fc(blockData, val, d->dstFinalRez.get(),static_cast<int>(blockSize));
    //    blockData=d->dstFinalRez.get();
}

int  ChannelEqualizer::calcFftOrder(quint32 number)
{
    int count = 2; //2 в 0 , 2 в 1
    quint32 bufValue = 2;
    for(int i = 2; i <= 16; i++)
    {
        bufValue *= 2;
        if(bufValue == number)
        {
            break;
        }
        count++;
    }
    qDebug() << bufValue << number;
    if(bufValue == number)
    {
        return count;
    }
    else
    {
        return -1;
    }
}

/*!
 * \brief SyncPairChannel::swapHalfPhaseValues
 * меняет местами левую половину массива с правой
 * \param phaseValues массив фаз сигнала
 * \param size
 */
void ChannelEqualizer::swapHalfPhaseValues(Ipp32f* phaseValues, quint32 size)const
{
    for(quint32 i = 0; i < size / 2; i++)
    {
        Ipp32f temp = phaseValues[i + size / 2];
        phaseValues[i + size / 2] = phaseValues[i];
        phaseValues[i] = temp;
    }
}

/*!
 * \brief SyncPairChannel::incrementPhaseValuesOnAngle
 * увеличение каждого элемента массива фаз на заданный угол
 * \param phaseData массив фаз сигнала
 * \param size
 * \param phaseAngle
 */
void ChannelEqualizer::incrementPhaseValuesOnAngle(Ipp32f* phaseData, quint32 size, float phaseAngle)const
{
    for(quint32 i = 0; i < size; i++)
    {
        phaseData[i] += phaseAngle;
    }
}
