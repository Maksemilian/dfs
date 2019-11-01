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
            shiftData.channelIndex=-1;
            shiftData.ddcDifference=0.0;

            shiftData.shiftBuffer.resize(blockSize);
        }

        quint32 sampleRate;
//    VectorIpp32fc shiftBuffer;
//    int channelIndex;
//    double ddcDifference;
//    double deltaStart;

    struct{
        VectorIpp32fc shiftBuffer;
        int channelIndex;
        double ddcDifference;
        double deltaStart;
    }shiftData;
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


void ShiftFinder::sync(const ShPtrPacketBufferPair buffers,
                       quint32 ddcFrequency,
                       quint32 sampleRate,
                       quint32 blockSize){

    qDebug()<<"THREAD_SYNC_BEGIN";

    d->shiftData.channelIndex=-1;
    d->shiftData.ddcDifference=0.0;
    d->shiftData.shiftBuffer.resize(blockSize);

    if(calcShiftInChannel(buffers,sampleRate)){
        BlockAlinement blockAlinement(d->shiftData.shiftBuffer,blockSize);
        SumSubMethod sumSubMethod(sampleRate,blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket=false;
        bool isSecondStationReadedPacket=false;

        qDebug()<<"CHANNEL_SHIFT:"<<getChannelIndex()
               <<"SHIFT_VALUE:"<<getShiftValue()
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
                         (packet[getChannelIndex()].sample().data()));

                blockAlinement.equate(signal,blockSize,d->shiftData.ddcDifference,
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
}

bool ShiftFinder::calcShiftInChannel(const ShPtrPacketBufferPair stationPair,
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

            d->shiftData.ddcDifference=  ddcDelayAfterLastPpsFirst-ddcDelayAfterLastPpsSecond;

            d->shiftData.channelIndex = d->shiftData.ddcDifference<0?CHANNEL_SECOND   :  CHANNEL_FIRST;
            d->shiftData.ddcDifference = abs(d->shiftData.ddcDifference);

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

            if(static_cast<quint32>(d->shiftData.ddcDifference)>packetPair[d->shiftData.channelIndex].block_size()){
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

            qDebug()<<"*********SHIFT_FOUND"<<d->shiftData.channelIndex
                   <<d->shiftData.ddcDifference
                  <<d->shiftData.deltaStart;
            shiftFound=true;
        }
    }

    return shiftFound;
}

void ShiftFinder::initShiftBuffer(const float *signalData, quint32 blockSize,
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

int ShiftFinder::getChannelIndex(){
    return d->shiftData.channelIndex;
}

const VectorIpp32fc& ShiftFinder::getShiftBuffer()
{
    return d->shiftData.shiftBuffer;
}

double ShiftFinder::getShiftValue()
{
    return d->shiftData.ddcDifference;
}

double ShiftFinder::getDeltaStart()
{
    return d->shiftData.deltaStart;
}
