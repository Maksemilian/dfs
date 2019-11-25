#include "sync_process.h"
#include "sync_block_equalizer.h"

#include "receiver.pb.h"

#include <math.h>

#include <QDebug>

#include "ipp.h"
#include "ippvm.h"

using PacketQueue=QQueue<proto::receiver::Packet>;
using PacketQueuePair=QPair<PacketQueue,PacketQueue>;

class SumSubMethod
{
    //1250 Гц - частота гетеродина создающего sin/cos коэффициенты
    const double HETERODYNE_FREQUENCY=1250.0;
    const double DOUBLE_PI=2*IPP_PI;
    const double DELTA_ANGLE_IN_RADIAN;
public:
    SumSubMethod(quint32 sampleRate,quint32 blockSize):
        DELTA_ANGLE_IN_RADIAN((360*(HETERODYNE_FREQUENCY/sampleRate))*(IPP_PI/180)),
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

struct SyncProcess::Impl
{
    Impl(){}
    Impl(ShPtrPacketBuffer syncBuffer1,
         ShPtrPacketBuffer syncBuffer2,
         ShPtrIpp32fcBuffer sumDivBuffer):
        syncBuffer1(syncBuffer1),syncBuffer2(syncBuffer2),sumDivBuffer(sumDivBuffer)
    {

    }
    Impl(quint32 sampleRate,quint32 blockSize):
        sampleRate(sampleRate)
    {
        shiftData.channelIndex=-1;
        shiftData.ddcDifference=0.0;

        shiftData.shiftBuffer.resize(blockSize);
    }

    quint32 sampleRate;

    struct
    {
        VectorIpp32fc shiftBuffer;
        int channelIndex;
        double ddcDifference;
        double deltaStart;
    }shiftData;

    ShPtrPacketBuffer syncBuffer1;
    ShPtrPacketBuffer syncBuffer2;
    ShPtrIpp32fcBuffer sumDivBuffer;

    std::atomic_bool quit;
};

SyncProcess::SyncProcess(const ShPtrPacketBuffer &syncBuffer1,
                         const ShPtrPacketBuffer &syncBuffer2,
                         const ShPtrIpp32fcBuffer &sumDivBuffer)
    :d(std::make_unique<Impl>(syncBuffer1,syncBuffer2,sumDivBuffer)){}

SyncProcess::~SyncProcess()
{
    qDebug()<<"SYNC_PROCESS_DESTR";
};


void SyncProcess::start(const ShPtrPacketBufferPair buffers,
                        quint32 ddcFrequency,
                        quint32 sampleRate,
                        quint32 blockSize){

    qDebug()<<"THREAD_SYNC_BEGIN";
    d->quit=false;
    d->shiftData.channelIndex=-1;
    d->shiftData.ddcDifference=0.0;
    d->shiftData.shiftBuffer.resize(blockSize);

    if(calcShiftInChannel(buffers,sampleRate)){
        BlockEqualizer blockEqualizer(d->shiftData.shiftBuffer,blockSize);
        SumSubMethod sumSubMethod(sampleRate,blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket=false;
        bool isSecondStationReadedPacket=false;

        qDebug()<<"CHANNEL_SHIFT:"<<d->shiftData.channelIndex
               <<"SHIFT_VALUE:"<<d->shiftData.ddcDifference
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
                         (packet[d->shiftData.channelIndex].sample().data()));

                blockEqualizer.equate(signal,blockSize,d->shiftData.ddcDifference,
                                      ddcFrequency,sampleRate,d->shiftData.deltaStart);
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
    emit finished();
}

void SyncProcess::stop()
{
    d->quit=true;
}

bool SyncProcess::calcShiftInChannel(const ShPtrPacketBufferPair stationPair,
                                     quint32 sampleRate)
{
    qDebug()<<"B_USE COUNT_calcShiftInChannel_BEGIN"
           <<stationPair.first.use_count()
          <<stationPair.first.use_count()
         <<sampleRate
        <<d->shiftData.shiftBuffer.size();

    BoolPair isReadedPacketPair;

    isReadedPacketPair.first=false;
    isReadedPacketPair.second=false;

    bool shiftFound=false;
    proto::receiver::Packet packetPair[2];

    proto::receiver::Packet packet;
    while(!shiftFound){
        //                qDebug()<<"read_1";
        if(stationPair.first->pop(packet)){
            if(packet.sample_rate()==sampleRate){
                packetPair[CHANNEL_FIRST]=packet;
                isReadedPacketPair.first=true;
                qDebug()<<"read_1"<<packet.block_number();
            }
        }
        //                qDebug()<<"read_2";
        if(stationPair.second->pop(packet)){
            if(packet.sample_rate()==sampleRate){
                packetPair[CHANNEL_SECOND]=packet;
                isReadedPacketPair.second=true;
                qDebug()<<"read_2"<<packet.block_number();
            }
        }

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
// 100 200
            d->shiftData.ddcDifference=  ddcDelayAfterLastPpsFirst-ddcDelayAfterLastPpsSecond;

            d->shiftData.channelIndex = d->shiftData.ddcDifference<0?
                        CHANNEL_SECOND   :  CHANNEL_FIRST;
            d->shiftData.ddcDifference = abs(d->shiftData.ddcDifference);

            //TODO поместить в один метод
            //BN1*BS1+SHIFT-BN2*BS2
            d->shiftData.deltaStart = d->shiftData.channelIndex==CHANNEL_FIRST ?
                        ((packetPair[CHANNEL_FIRST].block_number()*
                          packetPair[CHANNEL_FIRST].block_size())+ d->shiftData.ddcDifference)
                        -packetPair[CHANNEL_SECOND].block_number()*packetPair[CHANNEL_SECOND].block_size()
                      :
                        ((packetPair[CHANNEL_SECOND].block_number()*
                          packetPair[CHANNEL_SECOND].block_size())+ d->shiftData.ddcDifference)
                        -packetPair[CHANNEL_FIRST].block_number()*packetPair[CHANNEL_FIRST].block_size()
                        ;
            d->shiftData.deltaStart=abs(d->shiftData.deltaStart);

            Q_ASSERT_X( d->shiftData.ddcDifference>=0 &&  d->shiftData.ddcDifference<packet.block_size(),
                        "SignalSync::sync","error shift");

            if(static_cast<quint32>(d->shiftData.ddcDifference)>
                    packetPair[d->shiftData.channelIndex].block_size()){
                qDebug()<<"******************BAD SHIFT SIZE"
                       <<d->shiftData.ddcDifference
                      <<d->shiftData.deltaStart;
                isReadedPacketPair.first=false;
                isReadedPacketPair.second=false;
                continue;
            }

            initShiftBuffer(packetPair[d->shiftData.channelIndex].sample().data(),
                    packetPair[d->shiftData.channelIndex].block_size(),
                    static_cast<quint32>(d->shiftData.ddcDifference));

            //            initShiftBuffer(packetPair[d->channelIndex].sample().data(),
            //                    static_cast<quint32>(packetPair[d->channelIndex].sample().size()),
            //                    static_cast<quint32>(d->ddcDifference)*COUNT_SIGNAL_COMPONENT);

            showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
                       packetPair[CHANNEL_FIRST].time_of_week(),packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                       packetPair[CHANNEL_FIRST].adc_period_counter());

            showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
                       packetPair[CHANNEL_SECOND].time_of_week(),packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                       packetPair[CHANNEL_SECOND].adc_period_counter());

            qDebug()<<"*********SHIFT_FOUND"<<d->shiftData.channelIndex
                   <<d->shiftData.ddcDifference
                  <<d->shiftData.deltaStart;
            shiftFound=true;
        }
    }

    return shiftFound;
}

void SyncProcess::initShiftBuffer(const float *signalData, quint32 blockSize,
                                  quint32 shift)
{
    qDebug()<<"B_INIT_SHIFT_TEST"<<shift<<d->shiftData.shiftBuffer.size();
    for(quint32 i=0;i<shift;i++){
        d->shiftData.shiftBuffer[i].re=signalData[blockSize-(shift)+i*2];
        d->shiftData.shiftBuffer[i].im=signalData[(blockSize-(shift)+i*2)+1];
    }
}

/*!
 * \brief SyncPairChannel::ddcAfterLastPps
 * \param ddcSampleCounter
 * \param blockNumber
 * \param blockSize
 * \return колиество ddc сэмплов после прихода последнего импульса pps
 * Количество отсчетов после приема импульса PPS в каждом канале
 */
double SyncProcess::ddcAfterLastPps(double ddcSampleCounter,quint32 blockNumber,quint32 blockSize)
{
    return (blockNumber*blockSize)-ddcSampleCounter;
}



void SyncProcess::showPacket(quint32 blockNumber,
                             quint32 sampleRate,
                             quint32 timeOfWeek,
                             double ddcSampleCounter,
                             quint64 adcPeriodCounter)
{
    qDebug()<<"Packet"<<blockNumber<<sampleRate<<
              timeOfWeek<<ddcSampleCounter<<adcPeriodCounter;
}
