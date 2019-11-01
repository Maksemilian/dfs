#include "sync_shift_finder.h"
#include "sync_block_alinement.h"
#include "sync_sum_sub_method.h"

#include <vector>

#include <QDebug>
#include "receiver.pb.h"

using PacketQueue=QQueue<proto::receiver::Packet>;
using PacketQueuePair=QPair<PacketQueue,PacketQueue>;
//using PacketPair=QPair<proto::receiver::Packet,proto::receiver::Packet> ;
//******************************* FindChannelForShift ******************************************

struct ShiftFinder::Impl
{
    Impl(){}

    Impl(quint32 sampleRate,quint32 blockSize):
        sampleRate(sampleRate)
    {
        channelIndex=-1;
        ddcDifference=0.0;

        shiftBufferT.resize(blockSize);
    }

    quint32 sampleRate;
    VectorIpp32fc shiftBufferT;
    int channelIndex;
    double ddcDifference;
    double deltaStart;

    std::atomic_bool quit;
    ShPtrPacketBuffer syncBuffer1;
    ShPtrPacketBuffer syncBuffer2;
    ShPtrIpp32fcBuffer sumDivBuffer;
};

ShiftFinder::~ShiftFinder() = default;
void ShiftFinder::start()
{
    d->quit=false;
}

void ShiftFinder::stop()
{
    d->quit=true;
}

ShiftFinder::ShiftFinder():
    d(std::make_unique<Impl>()){}

ShiftFinder::ShiftFinder(quint32 sampleRate,quint32 blockSize):
    d(std::make_unique<Impl>(sampleRate,blockSize))
{}

void ShiftFinder::setBuffer1(const ShPtrPacketBuffer buffer1)
{
    d->syncBuffer1=buffer1;
}

void ShiftFinder::setBuffer2(const ShPtrPacketBuffer buffer2)
{
    d->syncBuffer2=buffer2;
}

void ShiftFinder::setSumDivBuffer(const ShPtrIpp32fcBuffer sumDivBuffer)
{
    d->sumDivBuffer=sumDivBuffer;
}

ShPtrPacketBuffer ShiftFinder::syncBuffer1()
{
    return d->syncBuffer1;
}

ShPtrPacketBuffer ShiftFinder::syncBuffer2()
{
    return d->syncBuffer2;
}

ShPtrIpp32fcBuffer ShiftFinder::sumDivMethod()
{
    return d->sumDivBuffer;
}

int ShiftFinder::getChannelIndex(){
    return d->channelIndex;
}

const VectorIpp32fc& ShiftFinder::getShiftBuffer()
{
    return d->shiftBufferT;
}

double ShiftFinder::getShiftValue()
{
    return d->ddcDifference;
}

double ShiftFinder::getDeltaStart()
{
    return d->deltaStart;
}

void ShiftFinder::sync(const ShPtrPacketBufferPair buffers,
                           quint32 ddcFrequency,
                           quint32 sampleRate,
                           quint32 blockSize){

    qDebug()<<"THREAD_SYNC_BEGIN";

    ShiftFinder f(sampleRate,blockSize);
    d->channelIndex=-1;
    d->ddcDifference=0.0;

    d->shiftBufferT.resize(blockSize);
    if(calcShiftInChannel(buffers,sampleRate)){
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

bool ShiftFinder::calcShiftInChannel(const ShPtrPacketBufferPair stationPair,
                                     quint32 sampleRate)
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
