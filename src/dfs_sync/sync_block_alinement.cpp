#include "sync_block_alinement.h"

#include "ipp.h"
#include "ippvm.h"

#include <memory>
#include <math.h>
#include <QDebug>

//WARNING - НАХОДИТЬСЯ В utility
using DeleterTypeIpp8u=std::function<void(Ipp8u*)>;

struct BlockAlinement::Impl
{
    Impl(const std::vector<Ipp32fc>&shiftBuffer,quint32 blockSize):
        dstFftFwd(VectorIpp32fc(blockSize)),
        dstMagn(VectorIpp32f(blockSize)),
        dstPhase(VectorIpp32f(blockSize)),
        dstCart32(VectorIpp32fc(blockSize)),
        dstFinalRez(VectorIpp32fc(blockSize)),
        shiftBufferT(shiftBuffer)
    {    }
    /// WARNING ЕСЛИ СДЕЛАТЬ С UNIQUE_PTR ТО ПРОГРАММА КРАШИТСЯ ПРИ ПЕРЕЗАПУСКЕ СИНХРОНИЗАЦИИ
    /// ИЗ-ЗА ТОГО ЧТО ИНТЕКЛЕКТУАЛЬНЫЕ УКАЗАТЕЛИ ИСПОЛЬЗУЮТ ОТЛИЧНЫЙ ОТ МЕХАНИЗМА IPP
    /// МЕХАНИЗМ ОСВОБОЖДЕНИЯ ПАМЯТИ

    DeleterTypeIpp8u deleterUniquePtrForIpp8u=[](Ipp8u*v){
        ippFree(v);
    };

    std::unique_ptr<Ipp8u,DeleterTypeIpp8u> pMemInit;
    std::unique_ptr<Ipp8u,DeleterTypeIpp8u> pMemBuffer;
    std::unique_ptr<Ipp8u,DeleterTypeIpp8u> pMemSpec;

    VectorIpp32fc dstFftFwd;
    VectorIpp32f dstMagn;
    VectorIpp32f dstPhase;
    VectorIpp32fc dstCart32;
    VectorIpp32fc dstFinalRez;

    VectorIpp32fc shiftBufferT;
};

BlockAlinement::BlockAlinement(const VectorIpp32fc &shiftBuffer,quint32 blockSize):
    d(std::make_unique<Impl>(shiftBuffer,blockSize))
{
    initFftBuffers(calcFftOrder(blockSize));
}

BlockAlinement::~BlockAlinement() = default;

void BlockAlinement::initFftBuffers(int FFTOrder)
{
    qDebug()<<"BlockAlinement::init FFTOrder"<<FFTOrder;
    Q_ASSERT_X(FFTOrder>0,"BlockAlinement::initFftBuffers","fft order below 0");

    int sizeSpec = 0;
    int sizeInit = 0;
    int sizeBuffer = 0;

    int flag =IPP_FFT_DIV_FWD_BY_N;
    ippsFFTGetSize_C_32fc(FFTOrder, flag, ippAlgHintNone,
                          &sizeSpec, &sizeInit, &sizeBuffer);

    std::unique_ptr<Ipp8u,DeleterTypeIpp8u>pMemSpecLocal(static_cast<Ipp8u*>(ippMalloc(sizeSpec)),
                                                         d->deleterUniquePtrForIpp8u);
    d->pMemSpec=std::move(pMemSpecLocal);
    if (sizeInit > 0){
        std::unique_ptr<Ipp8u,DeleterTypeIpp8u>pMemInitLocal(static_cast<Ipp8u*>(ippMalloc(sizeInit)),
                                                             d->deleterUniquePtrForIpp8u);
        d->pMemInit=std::move(pMemInitLocal);
    }
    if (sizeBuffer > 0){
        std::unique_ptr<Ipp8u,DeleterTypeIpp8u> pMemBufferLocal(static_cast<Ipp8u*>(ippMalloc(sizeBuffer)),
                                                                d->deleterUniquePtrForIpp8u);
        d->pMemBuffer=std::move(pMemBufferLocal);
    }

    ippsFFTInit_C_32fc(nullptr, FFTOrder, flag, ippAlgHintNone,d->pMemSpec.get(), d->pMemInit.get());
    qDebug()<<"BlockAlinement::init End";
}

void BlockAlinement::equate(Ipp32fc *blockData, quint32 blockSize,
                            double shift, quint32 ddcFrequency,
                            quint32 sampleRate, double deltaStart) const
{
    quint32 shiftW=static_cast<quint32>(shift);
    double shiftF=shift-shiftW;
    //    qDebug()<<"SHIFT"<<shift<<"W:"<<shiftW<<"F:"<<shiftF<<blockSize<<blockSize/2;
    //TODO ADD CONSTANT FOR NUMBER 2
    shiftWhole(blockData,blockSize,shiftW);

    //    qDebug()<<"SHIFT WHOLE END";

    shiftFruction(blockData,blockSize,shiftF);
    //    qDebug()<<"SHIFT FRUCTION END";
    double teta=(ddcFrequency/sampleRate)*2*M_PI*deltaStart;

    shiftTest(blockData,blockSize,teta);
    qDebug()<<"*******TETA:"<<teta<<ddcFrequency<<sampleRate<<deltaStart;
    //qDebug()<<"SHIFT FRUCTIOM END";
}

void BlockAlinement::shiftWhole(Ipp32fc *blockData, quint32 blockSize, quint32 shift) const
{
    Q_ASSERT_X(shift<blockSize,"SignalSync::shift64","shift > blockSize");

    std::vector<Ipp32fc> bufArray(blockSize);

    for(quint32 i=0;i<shift;i++){
        bufArray[i]=d->shiftBufferT[i];
        d->shiftBufferT[i]=blockData[blockSize-shift+i];
    }

    for(quint32 i=0;i<(blockSize-shift);i++)
        bufArray[i+shift]=blockData[i];

    for(quint32 i=0;i<blockSize;i++)
        blockData[i]=bufArray[i];
}

void BlockAlinement::shiftFruction(Ipp32fc *blockData, quint32 dataSize, double phaseAngle) const
{
    IppsFFTSpec_C_32fc *pSpec=reinterpret_cast<IppsFFTSpec_C_32fc *>(d->pMemSpec.get());
    //1---FFT FWD
    ippsFFTFwd_CToC_32fc(blockData, d->dstFftFwd.data(), pSpec, d->pMemBuffer.get());
    //2---CART TO POLAR
    ippsCartToPolar_32fc(d->dstFftFwd.data(),d->dstMagn.data(),d->dstPhase.data(),static_cast<int>(dataSize));
    //3--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.data(),dataSize);
    //4--SHIFT INCREMENT DST POLAR ON ADC
    incrementPhaseValuesOnAngle(d->dstPhase.data(),dataSize,static_cast<float>(phaseAngle));
    //5--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.data(),dataSize);
    //6--POLAR TO CART
    ippsPolarToCart_32fc(d->dstMagn.data(),d->dstPhase.data(),d->dstCart32.data(),static_cast<int>(dataSize));
    //7--FFT INV
    ippsFFTInv_CToC_32fc(d->dstCart32.data(),blockData,pSpec,d->pMemBuffer.get());
}

void BlockAlinement::shiftTest(Ipp32fc *blockData, quint32 blockSize, double teta) const
{
    Ipp32fc val;
    //    val.re=static_cast<float>(cos(teta));
    //    val.im=static_cast<float>(sin(teta));
    val.re=static_cast<float>(cos(fmod(teta,(2*M_PI))));
    val.im=static_cast<float>(sin(fmod(teta,(2*M_PI))));
    qDebug()<<"VAL"<<val.re<<val.im<<static_cast<quint64>(teta)<<fmod(teta,(2*M_PI));
    //WARNING ippsMulC_32fc
    ippsMulC_32fc(blockData, val, blockData,static_cast<int>(blockSize));
    //    ippsMulC_32fc(blockData, val, d->dstFinalRez.get(),static_cast<int>(blockSize));
    //    blockData=d->dstFinalRez.get();
}

int  BlockAlinement::calcFftOrder(quint32 number)
{
    int count=2;//2 в 0 , 2 в 1
    quint32 bufValue=2;
    for(int i=2;i<=16;i++){
        bufValue*=2;
        if(bufValue==number)
            break;
        count++;
    }
    qDebug()<<bufValue<<number;
    if(bufValue==number){
        return count;
    }else{
        return -1;
    }
}

/*!
 * \brief SyncPairChannel::swapHalfPhaseValues
 * меняет местами левую половину массива с правой
 * \param phaseValues массив фаз сигнала
 * \param size
 */
void BlockAlinement::swapHalfPhaseValues(Ipp32f *phaseValues, quint32 size)const
{
    for(quint32 i=0;i<size/2;i++){
        Ipp32f temp=phaseValues[i+size/2];
        phaseValues[i+size/2]=phaseValues[i];
        phaseValues[i]=temp;
    }
}

/*!
 * \brief SyncPairChannel::incrementPhaseValuesOnAngle
 * увеличение каждого элемента массива фаз на заданный угол
 * \param phaseData массив фаз сигнала
 * \param size
 * \param phaseAngle
 */
void BlockAlinement::incrementPhaseValuesOnAngle(Ipp32f *phaseData,quint32 size,float phaseAngle)const
{
    for(quint32 i=0;i<size;i++)
        phaseData[i]+=phaseAngle;
}
