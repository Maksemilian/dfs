#include "sync_pair_channel.h"

#include "i_sygnal_update.h"

#include "receiver.pb.h"

#include <vector>

#include <QQueue>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>

#include "ipp.h"
#include "ippvm.h"

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



BlockAlinement::BlockAlinement(const std::vector<Ipp32fc>&shiftBuffer,quint32 blockSize):
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

//*******************************************

using PacketQueue=QQueue<proto::receiver::Packet>;
using PacketQueuePair=QPair<PacketQueue,PacketQueue>;
using PacketPair=QPair<proto::receiver::Packet,proto::receiver::Packet> ;

class SumSubMethod
{
    //1250 Гц - частота гетеродина создающего sin/cos коэффициенты
    const double HETERODYNE_FREQUENCY=1250.0;
    const double DOUBLE_PI=2*M_PI;
    const double DELTA_ANGLE_IN_RADIAN;
public:
    SumSubMethod(quint32 sampleRate,quint32 blockSize):
        DELTA_ANGLE_IN_RADIAN((360*(HETERODYNE_FREQUENCY/sampleRate))*(M_PI/180)),
        im(VectorIpp32f(blockSize)),
        re(VectorIpp32f(blockSize)),
        complexSum(VectorIpp32fc(blockSize)),
        complexSub(VectorIpp32fc(blockSize)),
        mulSumOnCoef(VectorIpp32fc(blockSize)),
        mulSubOnCoef(VectorIpp32fc(blockSize)),
        dstSumDiv(VectorIpp32fc(blockSize)),
        currentAngleRad(0.0)
    {

    }

    const VectorIpp32fc& calc(const Ipp32fc *dst1,const Ipp32fc *dst2,quint32 blockSize)
    {
        //1 ***** Создаем коэффициенты
        for(quint32 i=0;i<blockSize;i++,currentAngleRad+=DELTA_ANGLE_IN_RADIAN){
            re[i]= static_cast<Ipp32f>(cos(currentAngleRad));
            im[i]= static_cast<Ipp32f>(sin(currentAngleRad));
            if(currentAngleRad>=DOUBLE_PI){
                re[i]=1;
                im[i]=0;
                currentAngleRad=0.0;
            }
        }

        ippsRealToCplx_32f(re.data(),im.data(),dstSumDiv.data(),
                           static_cast<int>(blockSize));

        // qDebug()<<"******************Src Coef:";

        //2 ***** Фильтруем 2 канала
        //WARNING ФИЛЬТРАЦИЯ НЕ ИСПОЛЬЗУЕТСЯ

        //2 ***** Комплексная сумма двух сигналов
        ippsAdd_32fc(dst1,dst2,complexSum.data(),static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER SUM:";

        //3 ***** Комплексная разность двух сигналов
        ippsSub_32fc(dst1,dst2,complexSub.data(),static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER SUB:";

        //4 ***** Умножить массив коэффициентов на сумму
        ippsMul_32fc(dstSumDiv.data(),complexSum.data(),mulSumOnCoef.data(),
                     static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER MUL SUM:";

        //5 ***** Умножить массив коэффициентов на разность
        ippsMul_32fc(dstSumDiv.data(),complexSub.data(),mulSubOnCoef.data(),
                     static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER MUL SUB:";

        //        for(quint32 i=0;i<blockSize;i++){
        //            dstSumDiv[i].re=mulSumOnCoef[i].re;
        //            dstSumDiv[i].im=mulSubOnCoef[i].im;
        //        }
        memcpy(dstSumDiv.data(),mulSubOnCoef.data(),sizeof (Ipp32fc)*blockSize);
        return  dstSumDiv;
    }

private:
    VectorIpp32f im;
    VectorIpp32f re;
    VectorIpp32fc complexSum;
    VectorIpp32fc complexSub;
    VectorIpp32fc mulSumOnCoef;
    VectorIpp32fc mulSubOnCoef;

    VectorIpp32fc dstSumDiv;
    double currentAngleRad;
};
//******************************* FindChannelForShift ******************************************

struct ShiftFinder::Impl
{
    Impl(quint32 sampleRate,quint32 blockSize):
        sampleRate(sampleRate)
    {
        channelIndex=-1;
        ddcDifference=0.0;

        shiftBuffer.resize(blockSize);
        shiftBufferT.resize(blockSize);
    }
    quint32 sampleRate;
    std::vector<float>shiftBuffer;
    VectorIpp32fc shiftBufferT;
    int channelIndex;
    double ddcDifference;
    double deltaStart;
};

ShiftFinder::~ShiftFinder() = default;

ShiftFinder::ShiftFinder(quint32 sampleRate,quint32 blockSize):
    d(std::make_unique<Impl>(sampleRate,blockSize))
{}

int ShiftFinder::getChannelIndex(){
    return d->channelIndex;
}

const VectorIpp32fc& ShiftFinder::getShiftBuffer()
{
    return d->shiftBufferT;
}

double ShiftFinder::getShiftValue(){
    return d->ddcDifference;
}

double ShiftFinder::getDeltaStart()
{
    return d->deltaStart;
}

bool ShiftFinder::calcShiftInChannel(const ShPtrBufferPair stationPair)
{
    qDebug()<<"B_USE COUNT_calcShiftInChannel_BEGIN"
           <<stationPair.first.use_count()
          <<stationPair.first.use_count()
         <<d->sampleRate
        <<d->shiftBufferT.size();

    BoolPair isReadedPacketPair;
    isReadedPacketPair.first=false;
    isReadedPacketPair.second=false;

    bool shiftFound=false;
    proto::receiver::Packet packetPair[2];

    proto::receiver::Packet packet;
    while(!shiftFound){
        //                qDebug()<<"read_1";
        if(stationPair.first->pop(packet)){
            if(packet.sample_rate()==d->sampleRate){
                packetPair[CHANNEL_FIRST]=packet;
                isReadedPacketPair.first=true;
                qDebug()<<"read_1"<<packet.block_number();
            }
        }
        //                qDebug()<<"read_2";
        if(stationPair.second->pop(packet)){
            if(packet.sample_rate()==d->sampleRate){
                packetPair[CHANNEL_SECOND]=packet;
                isReadedPacketPair.second=true;
                qDebug()<<"read_2"<<packet.block_number();
            }
        }
        //        qDebug()<<"Find_Shift_P1";
        //        showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
        //                   packetPair[CHANNEL_FIRST].time_of_week(),packetPair[CHANNEL_FIRST].ddc_sample_counter(),
        //                   packetPair[CHANNEL_FIRST].adc_period_counter());

        //        showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
        //                   packetPair[CHANNEL_SECOND].time_of_week(),packetPair[CHANNEL_SECOND].ddc_sample_counter(),
        //                   packetPair[CHANNEL_SECOND].adc_period_counter());
        //        qDebug()<<"Find_Shift_P2";
        if(isReadedPacketPair.first&&isReadedPacketPair.second){
            if(packetPair[CHANNEL_FIRST].time_of_week()!=
                    packetPair[CHANNEL_SECOND].time_of_week()){
                qDebug()<<"BAD EQUALS:"
                       <<packetPair[CHANNEL_FIRST].time_of_week()
                      <<packetPair[CHANNEL_SECOND].time_of_week();

                isReadedPacketPair.first=false;
                isReadedPacketPair.second=false;
                continue;
            }

            Q_ASSERT_X(packetPair[CHANNEL_FIRST].time_of_week() ==
                       packetPair[CHANNEL_SECOND].time_of_week(),
                       "SyncPairChannel::calcDdcDifferenceBetweenCannel","time of week not equ");

            double ddcDelayAfterLastPpsFirst=ddcAfterLastPps(packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                                                             packetPair[CHANNEL_FIRST].block_number(),
                                                             packetPair[CHANNEL_FIRST].block_size());

            double ddcDelayAfterLastPpsSecond=ddcAfterLastPps(packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                                                              packetPair[CHANNEL_SECOND].block_number(),
                                                              packetPair[CHANNEL_SECOND].block_size());

            d->ddcDifference=  ddcDelayAfterLastPpsFirst-ddcDelayAfterLastPpsSecond;

            d->channelIndex = d->ddcDifference<0?CHANNEL_SECOND   :  CHANNEL_FIRST;
            d->ddcDifference = abs(d->ddcDifference);

            d->deltaStart = d->channelIndex==CHANNEL_FIRST ?
                        ((packetPair[CHANNEL_FIRST].block_number()*
                          packetPair[CHANNEL_FIRST].block_size())+ d->ddcDifference)
                        -packetPair[CHANNEL_SECOND].block_number()*packetPair[CHANNEL_SECOND].block_size()
                      :
                        ((packetPair[CHANNEL_SECOND].block_number()*
                          packetPair[CHANNEL_SECOND].block_size())+ d->ddcDifference)
                        -packetPair[CHANNEL_FIRST].block_number()*packetPair[CHANNEL_FIRST].block_size()
                        ;
            d->deltaStart=abs(d->deltaStart);

            Q_ASSERT_X( d->ddcDifference>=0 &&  d->ddcDifference<packet.block_size(),
                        "SignalSync::sync","error shift");

            if(static_cast<quint32>(d->ddcDifference)>packetPair[d->channelIndex].block_size()){
                qDebug()<<"******************BAD SHIFT SIZE"
                       <<d->ddcDifference
                      <<d->deltaStart;
                isReadedPacketPair.first=false;
                isReadedPacketPair.second=false;
                continue;
            }

            initShiftBuffer(packetPair[d->channelIndex].sample().data(),
                    packetPair[d->channelIndex].block_size(),
                    static_cast<quint32>(d->ddcDifference));
            //WARNING UNCOMMENT THIS CODE
            //            initShiftBuffer(packetPair[d->channelIndex].sample().data(),
            //                    static_cast<quint32>(packetPair[d->channelIndex].sample().size()),
            //                    static_cast<quint32>(d->ddcDifference)*COUNT_SIGNAL_COMPONENT);

            showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
                       packetPair[CHANNEL_FIRST].time_of_week(),packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                       packetPair[CHANNEL_FIRST].adc_period_counter());

            showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
                       packetPair[CHANNEL_SECOND].time_of_week(),packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                       packetPair[CHANNEL_SECOND].adc_period_counter());

            qDebug()<<"*********SHIFT_FOUND"<<d->channelIndex
                   <<d->ddcDifference
                  <<d->deltaStart;
            shiftFound=true;
        }
    }
    //    qDebug()<<"B_USE COUNT_calcShiftInChannel_END"<<stationPair.first.use_count()
    //           <<stationPair.first.use_count();
    //    showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
    //               packetPair[CHANNEL_FIRST].time_of_week(),packetPair[CHANNEL_FIRST].ddc_sample_counter(),
    //               packetPair[CHANNEL_FIRST].adc_period_counter());

    //    showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
    //               packetPair[CHANNEL_SECOND].time_of_week(),packetPair[CHANNEL_SECOND].ddc_sample_counter(),
    //               packetPair[CHANNEL_SECOND].adc_period_counter());
    //    qDebug()<<"****************FIND SHIFT****************";
    //    qDebug()<<"USE COUNT_1"<<stationPair.first.use_count()<<stationPair.first.use_count();
    return shiftFound;
}



void ShiftFinder::initShiftBuffer(const float *signalData, quint32 blockSize,
                                          quint32 shift)
{
    qDebug()<<"B_INIT_SHIFT_TEST"<<shift<<d->shiftBufferT.size();
    for(quint32 i=0;i<shift;i++){
        d->shiftBufferT[i].re=signalData[blockSize-(shift)+i*2];
        d->shiftBufferT[i].im=signalData[(blockSize-(shift)+i*2)+1];
    }
}

/*!
 * \brief SyncPairChannel::ddcAfterLastPps
 * \param ddcSampleCounter
 * \param blockNumber
 * \param blockSize
 * \return колиество ddc сэмплов после прихода последнего импульса pps
 */
double ShiftFinder::ddcAfterLastPps(double ddcSampleCounter,quint32 blockNumber,quint32 blockSize)
{
    return (blockNumber*blockSize)-ddcSampleCounter;
}

void ShiftFinder::showPacket(quint32 blockNumber,
                                     quint32 sampleRate,
                                     quint32 timeOfWeek,
                                     double ddcSampleCounter,
                                     quint64 adcPeriodCounter)
{
    qDebug()<<"Packet"<<blockNumber<<sampleRate<<
              timeOfWeek<<ddcSampleCounter<<adcPeriodCounter;
}

//******************************* SyncPairChannel *************************

struct SyncPairChannel::Impl
{
    Impl():
        isWholeShiftEnabled(true),
        isFructionShiftEnabled(true),
        syncBuffer1(std::make_shared<RingBuffer>(16)),
        syncBuffer2(std::make_shared<RingBuffer>(16)),
        sumDivBuffer(std::make_shared<RingBufferT<std::vector<Ipp32fc>>>(16))

    {}

    std::atomic<bool> isWholeShiftEnabled;
    std::atomic<bool> isFructionShiftEnabled;
    QFutureWatcher<void> fw;
    std::atomic<bool> quit;

    std::shared_ptr<RingBuffer>syncBuffer1;
    std::shared_ptr<RingBuffer>syncBuffer2;
    std::shared_ptr<RingBufferT<std::vector<Ipp32fc>> > sumDivBuffer;
};

SyncPairChannel::SyncPairChannel():
    d(std::make_unique<Impl>())
{}

ShPtrBuffer SyncPairChannel::syncBuffer1()
{
    return d->syncBuffer1;
}

ShPtrBuffer SyncPairChannel::syncBuffer2()
{
    return d->syncBuffer2;
}

ShPtrBufferT SyncPairChannel::sumDivMethod()
{
    return d->sumDivBuffer;
}

SyncPairChannel::~SyncPairChannel()= default;


//************* START / STOP SYNC *************************************
/*!
 * \brief SyncPairChannel::sync
 * процесс синхронизации двух каналов
 */

void SyncPairChannel::sync(const ShPtrBufferPair buffers,
                           quint32 ddcFrequency,
                           quint32 sampleRate,
                           quint32 blockSize){

    qDebug()<<"THREAD_SYNC_BEGIN";

    ShiftFinder f(sampleRate,blockSize);
    if(f.calcShiftInChannel(buffers)){
        BlockAlinement blockAlinement(f.getShiftBuffer(),blockSize);
        SumSubMethod sumSubMethod(sampleRate,blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket=false;
        bool isSecondStationReadedPacket=false;

        qDebug()<<"CHANNEL_SHIFT:"<<f.getChannelIndex()
               <<"SHIFT_VALUE:"<<f.getShiftValue()
              <<"BUF_USE_COUNT"<<buffers.first.use_count()<<buffers.second.use_count();
        while(!d->quit){
            if(buffers.first->pop(packet[CHANNEL_FIRST])){
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket=true;
            }

            if(buffers.second->pop(packet[CHANNEL_SECOND])){
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket=true;
            }

            if(isFirstStationReadedPacket&&isSecondStationReadedPacket){
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST]=syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND]=syncQueuePair.second.dequeue();

                Ipp32fc *signal=reinterpret_cast<Ipp32fc*>
                        (const_cast<float*>
                         (packet[f.getChannelIndex()].sample().data()));

                blockAlinement.equate(signal,blockSize,f.getShiftValue(),
                                      ddcFrequency,sampleRate,f.getDeltaStart());
                //                                qDebug()<<"BA_2";
                d->syncBuffer1->push(packet[CHANNEL_FIRST]);
                d->syncBuffer2->push(packet[CHANNEL_SECOND]);
                //

                //******* SUM-SUB METHOD **************
                const Ipp32fc *dst1=reinterpret_cast<const Ipp32fc*>
                        (packet[CHANNEL_FIRST].sample().data());

                const Ipp32fc *dst2=reinterpret_cast<const Ipp32fc*>
                        (packet[CHANNEL_SECOND].sample().data());

                d->sumDivBuffer->push(sumSubMethod.
                                      calc(dst1,dst2,blockSize));

                isFirstStationReadedPacket=false;
                isSecondStationReadedPacket=false;
            }
        }
    }else  qDebug("SYNC_ERROR");

    qDebug()<<"THREAD_SYNC_END";
}


/*!
 * \brief SyncPairChannel::start
 * запускает процесс синхронизации двух каналов
 * \param receiverStationClientPair
 * \param blockSize
 */
void SyncPairChannel::start(const ShPtrBufferPair receiverStationClientPair,
                            quint32 ddcFrequency,
                            quint32 sampleRate,
                            quint32 blockSize)
{
    qDebug()<<"******SyncPairChannel::start();";
    qDebug()<<"COUNT"<<receiverStationClientPair.second.use_count();
    d->quit=false;
    d->fw.setFuture(QtConcurrent::run(this,&SyncPairChannel::sync,
                                      receiverStationClientPair,
                                      ddcFrequency,sampleRate,blockSize));
}

/*!
 * \brief SyncPairChannel::stop
 * останавливает процесс синхронизации двух каналов
 * \param reset
 * \todo убрать параметр reset . Этот парамер необходим когда перезапускаем
 * синхронизацию и должен быть равен false
 */
void SyncPairChannel::stop()
{
    d->quit=true;
    d->fw.waitForFinished();
    qDebug()<<"SyncPairChannel::stop();";
}

void SyncPairChannel::enableWholeShift(){
    d->isWholeShiftEnabled=true;
    qDebug()<<"ENABLED WHOLE SHIFT";
}

void SyncPairChannel::disableWholeShift(){
    d->isWholeShiftEnabled=false;
    qDebug()<<"DISABLED WHOLE SHIFT";
}

void SyncPairChannel::enableFructionShift(){
    d->isFructionShiftEnabled=true;
    qDebug()<<"ENABLED FRUCTION SHIFT";
}

void SyncPairChannel::disabledFructionShift(){
    d->isFructionShiftEnabled=false;
    qDebug()<<"DISABLED FRUCTION SHIFT";
}

//                memcpy(sumSubData.get(),reinterpret_cast<float*>(dstSumDivCoef.get()),
//                       sizeof (float)*blockSize*COUNT_SIGNAL_COMPONENT);

//                memcpy(v.data(),dstSumDivCoef.get(),sizeof (Ipp32fc)*blockSize);
//void SyncPairChannel::sync(const std::shared_ptr<RingBuffer> buffer1,
//                           const std::shared_ptr<RingBuffer> buffer2,
//                           quint32 ddcFrequency,
//                           quint32 sampleRate,
//                           quint32 blockSize)
//{
//    qDebug()<<"THREAD_SYNC_BEGIN";
//    //         qDebug()<<"USE COUNT_B"<<stationPair.first.use_count()<<stationPair.first.use_count();
//    FindChannelForShift f(sampleRate,blockSize);
//    // qDebug()<<"USE COUNT_"<<stationPair.first.use_count()<<stationPair.first.use_count();
//    if(f.calcShiftInChannel(stationPair)){
//        qDebug()<<"USE COUNT_E"<<stationPair.first.use_count()<<stationPair.first.use_count();
//        BlockAlinement blockAlinement(f.getShiftBuffer(),blockSize);
//        qDebug()<<"BlockAlinment is ready";
//        SumSubMethod sumSubMethod(sampleRate,blockSize);
//        qDebug()<<"Init is done:"<<f.getChannelIndex()<<f.getShiftValue();

//        proto::receiver::Packet packet[CHANNEL_SIZE];
//        PacketQueuePair syncQueuePair;

//        bool isFirstStationReadedPacket=false;
//        bool isSecondStationReadedPacket=false;

//        std::unique_ptr<float[]>dataPairSingal(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
//        std::unique_ptr<float[]>sumSubData(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
//        qDebug()<<"Start Circle";
//        qDebug()<<"USE COUNT_SC"<<stationPair.first.use_count()<<stationPair.first.use_count();
//        while(!d->quit){
//            if(stationPair.first->pop(packet[CHANNEL_FIRST])){
//                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
//                isFirstStationReadedPacket=true;
//            }

//            if(stationPair.second->pop(packet[CHANNEL_SECOND])){
//                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
//                isSecondStationReadedPacket=true;
//            }

//            if(isFirstStationReadedPacket&&isSecondStationReadedPacket){
//                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
//                packet[CHANNEL_FIRST]=syncQueuePair.first.dequeue();
//                packet[CHANNEL_SECOND]=syncQueuePair.second.dequeue();

//                float*data= const_cast<float*>(packet[f.getChannelIndex()].sample().data());
//                //                qDebug()<<"BA_1"<<f.getShiftValue();
//                Ipp32fc *signal=reinterpret_cast<Ipp32fc*>(data);

//                blockAlinement.equate(signal,blockSize,f.getShiftValue(),
//                                      ddcFrequency,sampleRate,f.getDeltaStart());
//                //                qDebug()<<"BA_2";
//                ChannelDataT channelData1(packet[CHANNEL_FIRST].block_number(),
//                                          packet[CHANNEL_FIRST].ddc_sample_counter(),
//                                          packet[CHANNEL_FIRST].adc_period_counter());

//                ChannelDataT channelData2(packet[CHANNEL_SECOND].block_number(),
//                                          packet[CHANNEL_SECOND].ddc_sample_counter(),
//                                          packet[CHANNEL_SECOND].adc_period_counter());

//                const float *data1=packet[CHANNEL_FIRST].sample().data();
//                const float *data2=packet[CHANNEL_SECOND].sample().data();
//                //В dataPairSingal заносятся I компоненты с канала 1 и 2
//                for(quint32 i=0;i<blockSize;i++){
//                    dataPairSingal[i*2]=data1[i*2];
//                    dataPairSingal[i*2+1]=data2[i*2];
//                }

//                //******* SUM-SUB METHOD **************
//                const std::unique_ptr<Ipp32fc[]>&dstSumDivCoef=sumSubMethod.calc(data1,data2,blockSize);
//                memcpy(sumSubData.get(),reinterpret_cast<float*>(dstSumDivCoef.get()),
//                       sizeof (float)*blockSize*COUNT_SIGNAL_COMPONENT);

//                if(d->syncSignalUpdater!=nullptr){
//                    d->syncSignalUpdater->updateSignalData(INDEX,channelData1,channelData2);
//                    d->syncSignalUpdater->updateSignalComponent(INDEX,dataPairSingal.get(),blockSize);
//                }

//                if(d->sumDivUpdater!=nullptr)
//                    d->sumDivUpdater->update(INDEX,sumSubData.get(),blockSize);

//                isFirstStationReadedPacket=false;
//                isSecondStationReadedPacket=false;
//            }
//            //             QThread::msleep(1000/(sampleRate/blockSize));
//        }
//    }else{
//        qDebug("SYNC_ERROR");
//    }
//    qDebug()<<"THREAD_SYNC_END";
//}

/*
void SyncPairChannel::sync(const StreamReadablePair stationPair,
                           quint32 ddcFrequency,quint32 sampleRate,quint32 blockSize)
{
    qDebug()<<"THREAD_SYNC_BEGIN";
    //         qDebug()<<"USE COUNT_B"<<stationPair.first.use_count()<<stationPair.first.use_count();
    //WARNING ЗДЕСЬ ПЕРЕДАЕТСЯ 16384
    FindChannelForShift f(sampleRate, COUNT_SIGNAL_COMPONENT*blockSize);
    // qDebug()<<"USE COUNT_"<<stationPair.first.use_count()<<stationPair.first.use_count();
    if(f.calcShiftInChannel(stationPair)){
        qDebug()<<"USE COUNT_E"<<stationPair.first.use_count()<<stationPair.first.use_count();
        //WARNING ЗДЕСЬ ПЕРЕДАЕТСЯ 8192
        BlockAlinement blockAlinement(f.getShiftBuffer(),blockSize);
        qDebug()<<"BlockAlinment is ready";
        SumSubMethod sumSubMethod(sampleRate,blockSize);
        qDebug()<<"Init is done:"<<f.getChannelIndex()<<f.getShiftValue();

        Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket=false;
        bool isSecondStationReadedPacket=false;

        std::unique_ptr<float[]>dataPairSingal(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
        std::unique_ptr<float[]>sumSubData(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
        qDebug()<<"Start Circle";
        qDebug()<<"USE COUNT_SC"<<stationPair.first.use_count()<<stationPair.first.use_count();
        while(!d->quit){
            if(stationPair.first->readDDC1StreamData(packet[CHANNEL_FIRST])){
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket=true;
            }

            if(stationPair.second->readDDC1StreamData(packet[CHANNEL_SECOND])){
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket=true;
            }

            if(isFirstStationReadedPacket&&isSecondStationReadedPacket){
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST]=syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND]=syncQueuePair.second.dequeue();

                float*data= const_cast<float*>(packet[f.getChannelIndex()].sample().data());
                //                                qDebug()<<"BA_1"<<f.getShiftValue();

                blockAlinement.equate(data,blockSize*COUNT_SIGNAL_COMPONENT,f.getShiftValue(),
                                      ddcFrequency,sampleRate,f.getDeltaStart(),
                                      d->isWholeShiftEnabled,d->isFructionShiftEnabled);
                //                                qDebug()<<"BA_2";
                ChannelDataT channelData1(packet[CHANNEL_FIRST].block_number(),
                                          packet[CHANNEL_FIRST].ddc_sample_counter(),
                                          packet[CHANNEL_FIRST].adc_period_counter());

                ChannelDataT channelData2(packet[CHANNEL_SECOND].block_number(),
                                          packet[CHANNEL_SECOND].ddc_sample_counter(),
                                          packet[CHANNEL_SECOND].adc_period_counter());

                const float *data1=packet[CHANNEL_FIRST].sample().data();
                const float *data2=packet[CHANNEL_SECOND].sample().data();
                //В dataPairSingal заносятся I компоненты с канала 1 и 2
                for(quint32 i=0;i<blockSize;i++){
                    dataPairSingal[i*2]=data1[i*2];
                    dataPairSingal[i*2+1]=data2[i*2];
                }

                const std::unique_ptr<Ipp32fc[]>&dstSumDivCoef=sumSubMethod.calc(data1,data2,blockSize);
                memcpy(sumSubData.get(),reinterpret_cast<float*>(dstSumDivCoef.get()),
                       sizeof (float)*blockSize*COUNT_SIGNAL_COMPONENT);

                if(d->syncSignalUpdater!=nullptr){
                    d->syncSignalUpdater->updateSignalData(INDEX,channelData1,channelData2);
                    d->syncSignalUpdater->updateSignalComponent(INDEX,dataPairSingal.get(),blockSize);
                }

                if(d->sumDivUpdater!=nullptr)
                    d->sumDivUpdater->update(INDEX,sumSubData.get(),blockSize);

                isFirstStationReadedPacket=false;
                isSecondStationReadedPacket=false;
            }
            //             QThread::msleep(1000/(sampleRate/blockSize));
        }
    }else{
        qDebug("SYNC_ERROR");
    }
    qDebug()<<"THREAD_SYNC_END";
}
*/
/*
void FindChannelForShift::initShiftBuffer(const float *signalData,quint32 blockSize,quint32 shift)
{
    qDebug()<<"B_INIT_SHIFT"<<shift<<d->shiftBuffer.size();
    for(quint32 i=0;i<shift;i++)
        d->shiftBuffer[i]=signalData[blockSize-(shift)+i];

    //    qDebug()<<"E_INIT_SHIFT"<<shift<<d->shiftBuffer.size();
}
FOR SYNC FROM FILE
void SyncPairChannel::sync(const StreamReadablePair stationPair,
                           quint32 ddcFrequency,quint32 sampleRate,quint32 blockSize)
{
    qDebug()<<"THREAD_SYNC_BEGIN";
    //         qDebug()<<"USE COUNT_B"<<stationPair.first.use_count()<<stationPair.first.use_count();
    //WARNING ЗДЕСЬ ПЕРЕДАЕТСЯ 16384
    FindChannelForShift f(sampleRate, COUNT_SIGNAL_COMPONENT*blockSize);
    // qDebug()<<"USE COUNT_"<<stationPair.first.use_count()<<stationPair.first.use_count();
    if(f.calcShiftInChannel(stationPair)){
        qDebug()<<"USE COUNT_E"<<stationPair.first.use_count()<<stationPair.first.use_count();
        //WARNING ЗДЕСЬ ПЕРЕДАЕТСЯ 8192
        BlockAlinement blockAlinement(f.getShiftBuffer(),blockSize);
        qDebug()<<"BlockAlinment is ready";
        SumSubMethod sumSubMethod(sampleRate,blockSize);
        qDebug()<<"Init is done:"<<f.getChannelIndex()<<f.getShiftValue();

        Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket=false;
        bool isSecondStationReadedPacket=false;

        std::unique_ptr<float[]>dataPairSingal(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
        std::unique_ptr<float[]>sumSubData(new float[blockSize*COUNT_SIGNAL_COMPONENT]);
        qDebug()<<"Start Circle";
        qDebug()<<"USE COUNT_SC"<<stationPair.first.use_count()<<stationPair.first.use_count();
        while(!d->quit){
            //            qDebug()<<"iteration"<<packet->block_number()<<packet->block_number()
            //                   <<syncQueuePair.first.size()<<syncQueuePair.second.size();
            // qDebug()<<"USE COUNT"<<stationPair.first.use_count()<<stationPair.first.use_count();
            if(stationPair.first->readDDC1StreamData(packet[CHANNEL_FIRST])){
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket=true;
                //                qDebug()<<"Packet read:";
                //                FindChannelForShift::showPacket(
                //                           packet[CHANNEL_FIRST].block_number(),packet[CHANNEL_FIRST].sample_rate(),
                //                           packet[CHANNEL_FIRST].time_of_week(),packet[CHANNEL_FIRST].ddc_sample_counter(),
                //                           packet[CHANNEL_FIRST].adc_period_counter());
            }

            if(stationPair.second->readDDC1StreamData(packet[CHANNEL_SECOND])){
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket=true;

                //                FindChannelForShift::showPacket(
                //                           packet[CHANNEL_SECOND].block_number(),packet[CHANNEL_SECOND].sample_rate(),
                //                           packet[CHANNEL_SECOND].time_of_week(),packet[CHANNEL_SECOND].ddc_sample_counter(),
                //                           packet[CHANNEL_SECOND].adc_period_counter());
            }

            //            qDebug()<<"Block_B";
            //            FindChannelForShift::showPacket(
            //                       packet[CHANNEL_FIRST].block_number(), packet[CHANNEL_FIRST].sample_rate(),
            //                       packet[CHANNEL_FIRST].time_of_week(),packet[CHANNEL_FIRST].ddc_sample_counter(),
            //                       packet[CHANNEL_FIRST].adc_period_counter());

            //            FindChannelForShift::showPacket(
            //                       packet[CHANNEL_SECOND].block_number(), packet[CHANNEL_SECOND].sample_rate(),
            //                       packet[CHANNEL_SECOND].time_of_week(),packet[CHANNEL_SECOND].ddc_sample_counter(),
            //                       packet[CHANNEL_SECOND].adc_period_counter());
            //            qDebug()<<"Block_E";

            if(isFirstStationReadedPacket&&isSecondStationReadedPacket){
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST]=syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND]=syncQueuePair.second.dequeue();
                //                qDebug()<<"Is find_E";
                //                qDebug()<<"Block_Q_B";
                //                FindChannelForShift::showPacket(packet[CHANNEL_FIRST].block_number(), packet[CHANNEL_FIRST].sample_rate(),
                //                           packet[CHANNEL_FIRST].time_of_week(),packet[CHANNEL_FIRST].ddc_sample_counter(),
                //                           packet[CHANNEL_FIRST].adc_period_counter());

                //                FindChannelForShift::showPacket(packet[CHANNEL_SECOND].block_number(), packet[CHANNEL_SECOND].sample_rate(),
                //                           packet[CHANNEL_SECOND].time_of_week(),packet[CHANNEL_SECOND].ddc_sample_counter(),
                //                           packet[CHANNEL_SECOND].adc_period_counter());
                //                qDebug()<<"Block_Q_E";
                //                qDebug()<<"COUNT===:"<<stationPair.first.use_count()<<stationPair.second.use_count();

                float*data= const_cast<float*>(packet[f.getChannelIndex()].sample().data());
                //                                qDebug()<<"BA_1"<<f.getShiftValue();

                blockAlinement.equate(data,blockSize*COUNT_SIGNAL_COMPONENT,f.getShiftValue(),
                                      ddcFrequency,sampleRate,f.getDeltaStart(),
                                      d->isWholeShiftEnabled,d->isFructionShiftEnabled);
                //                                qDebug()<<"BA_2";
                ChannelDataT channelData1(packet[CHANNEL_FIRST].block_number(),
                                          packet[CHANNEL_FIRST].ddc_sample_counter(),
                                          packet[CHANNEL_FIRST].adc_period_counter());

                ChannelDataT channelData2(packet[CHANNEL_SECOND].block_number(),
                                          packet[CHANNEL_SECOND].ddc_sample_counter(),
                                          packet[CHANNEL_SECOND].adc_period_counter());

                const float *data1=packet[CHANNEL_FIRST].sample().data();
                const float *data2=packet[CHANNEL_SECOND].sample().data();
                //В dataPairSingal заносятся I компоненты с канала 1 и 2
                for(quint32 i=0;i<blockSize;i++){
                    dataPairSingal[i*2]=data1[i*2];
                    dataPairSingal[i*2+1]=data2[i*2];
                }

                const std::unique_ptr<Ipp32fc[]>&dstSumDivCoef=sumSubMethod.calc(data1,data2,blockSize);
                memcpy(sumSubData.get(),reinterpret_cast<float*>(dstSumDivCoef.get()),
                       sizeof (float)*blockSize*COUNT_SIGNAL_COMPONENT);

                //                SyncSignalPairChannelEventPublisher::update(dataPairSingal.get(),blockSize,channelData1,channelData2);
                //                SumDivMethodEventPublisher::update(sumSubData.get(),blockSize);

                //                ISyncSignalUpdate::update(INDEX,dataPairSingal.get(),blockSize,channelData1,channelData2);
                //                ISumDivSignalUpdate::update(INDEX,sumSubData.get(),blockSize);

                if(d->syncSignalUpdater!=nullptr){
                    d->syncSignalUpdater->updateSignalData(INDEX,channelData1,channelData2);
                    d->syncSignalUpdater->updateSignalComponent(INDEX,dataPairSingal.get(),blockSize);
                }

                if(d->sumDivUpdater!=nullptr)
                    d->sumDivUpdater->update(INDEX,sumSubData.get(),blockSize);

                isFirstStationReadedPacket=false;
                isSecondStationReadedPacket=false;
            }
            //             QThread::msleep(1000/(sampleRate/blockSize));
        }
    }else{
        qDebug("SYNC_ERROR");
    }
    qDebug()<<"THREAD_SYNC_END";
}

*/

/*

void BlockAlinement::equate(float *blockData, quint32 blockSize,double shift,
                            quint32 ddcFrequency,quint32 sampleRate,double deltaStart,
                            bool isWholeShift,bool isFructionShift) const
{
    quint32 shiftW=static_cast<quint32>(shift);
    double shiftF=shift-shiftW;
    //    qDebug()<<"SHIFT"<<shift<<"W:"<<shiftW<<"F:"<<shiftF<<blockSize<<blockSize/2;
    //TODO ADD CONSTANT FOR NUMBER 2
    if(isWholeShift)
        shiftWhole(blockData,blockSize,shiftW*2);

    //qDebug()<<"SHIFT WHOLE END";

    if(isFructionShift)
        shiftFruction(blockData,blockSize/2,shiftF);

    double teta=(ddcFrequency/sampleRate*(2*M_PI))*deltaStart;

    shiftTest(blockData,blockSize/2,teta);
    qDebug()<<"*******TETA:"<<teta<<ddcFrequency<<sampleRate<<deltaStart;
    //qDebug()<<"SHIFT FRUCTIOM END";
}

BlockAlinement::BlockAlinement(const std::vector<float>&shiftBuffer,quint32 blockSize):
    d(std::make_unique<Impl>(shiftBuffer,blockSize))
{
    initFftBuffers(calcFftOrder(blockSize));
}

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

void BlockAlinement::shiftWhole(float *blockData, quint32 blockSize, quint32 shift)const
{
    //        qDebug()<<"whole_shift";
    Q_ASSERT_X(shift<blockSize,"SignalSync::shift64","shift > blockSize");

    std::vector<float> bufArray(blockSize);

    for(quint32 i=0;i<shift;i++){
        bufArray[i]=d->shiftBuffer[i];
        d->shiftBuffer[i]=blockData[blockSize-shift+i];
    }

    for(quint32 i=0;i<(blockSize-shift);i++)
        bufArray[i+shift]=blockData[i];

    for(quint32 i=0;i<blockSize;i++)
        blockData[i]=bufArray[i];
}

void BlockAlinement::shiftFruction(float *blockData, quint32 dataSize, double phaseAngle)const
{
    //        qDebug()<<"fruction_shift";
    Ipp32fc *Src=reinterpret_cast<Ipp32fc*>(blockData);
    IppsFFTSpec_C_32fc *pSpec=reinterpret_cast<IppsFFTSpec_C_32fc *>(d->pMemSpec.get());
    //1---FFT FWD
    ippsFFTFwd_CToC_32fc(Src, d->dstFftFwd.get(), pSpec, d->pMemBuffer.get());
    //2---CART TO POLAR
    ippsCartToPolar_32fc(d->dstFftFwd.get(),d->dstMagn.get(),d->dstPhase.get(),static_cast<int>(dataSize));
    //3--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.get(),dataSize);
    //4--SHIFT INCREMENT DST POLAR ON ADC
    incrementPhaseValuesOnAngle(d->dstPhase.get(),dataSize,static_cast<float>(phaseAngle));
    //5--SWAP PART BLOCK SIGNAL
    swapHalfPhaseValues(d->dstPhase.get(),dataSize);
    //6--POLAR TO CART
    ippsPolarToCart_32fc(d->dstMagn.get(),d->dstPhase.get(),d->dstCart32.get(),static_cast<int>(dataSize));
    //7--FFT INV
    ippsFFTInv_CToC_32fc(d->dstCart32.get(),Src,pSpec,d->pMemBuffer.get());
    blockData=reinterpret_cast<float*>(Src);
}

void BlockAlinement::shiftTest(float *blockData, quint32 blockSize, double teta)const
{
    Ipp32fc *Src=reinterpret_cast<Ipp32fc*>(blockData);
    Ipp32fc val;
    val.re=static_cast<float>(cos(teta));
    val.im=static_cast<float>(sin(teta));

    //    qDebug()<<"SHIFT TEST:"<<quint64(teta)<<val.re<<val.im<<cos(teta)<<sin(teta);
    ippsMulC_32fc(Src, val, d->dstFinalRez.get(),static_cast<int>(blockSize));
    blockData=reinterpret_cast<float*>(d->dstFinalRez.get());
}
*/
