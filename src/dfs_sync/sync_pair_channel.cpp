#include "sync_pair_channel.h"
#include "sync_shift_finder.h"
#include "sync_block_alinement.h"
#include "sync_sum_sub_method.h"

#include <QQueue>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>
#include <QPair>
//*******************************************

using PacketQueue=QQueue<proto::receiver::Packet>;
using PacketQueuePair=QPair<PacketQueue,PacketQueue>;
using PacketPair=QPair<proto::receiver::Packet,proto::receiver::Packet> ;

//******************************* SyncPairChannel *************************

struct SyncPairChannel::Impl
{
    Impl():
        isWholeShiftEnabled(true),
        isFructionShiftEnabled(true),
        syncBuffer1(std::make_shared<RingPacketBuffer>(16)),
        syncBuffer2(std::make_shared<RingPacketBuffer>(16)),
        sumDivBuffer(std::make_shared<RingIpp32fcBuffer>(16))
    {}

    std::atomic_bool isWholeShiftEnabled;//
    std::atomic_bool isFructionShiftEnabled;//

    std::atomic_bool quit;
    QFutureWatcher<void> fw;

    ShPtrPacketBuffer syncBuffer1;
    ShPtrPacketBuffer syncBuffer2;
    ShPtrIpp32fcBuffer sumDivBuffer;
    ShiftFinder *shiftFinder;
};

SyncPairChannel::SyncPairChannel():
    d(std::make_unique<Impl>())
{}

ShPtrPacketBuffer SyncPairChannel::syncBuffer1()
{
    return d->syncBuffer1;
}

ShPtrPacketBuffer SyncPairChannel::syncBuffer2()
{
    return d->syncBuffer2;
}

ShPtrIpp32fcBuffer SyncPairChannel::sumDivMethod()
{
    return d->sumDivBuffer;
}

SyncPairChannel::~SyncPairChannel()= default;


//************* START / STOP SYNC *************************************
/*!
 * \brief SyncPairChannel::sync
 * процесс синхронизации двух каналов
 */

void SyncPairChannel::sync(const ShPtrPacketBufferPair buffers,
                           quint32 ddcFrequency,
                           quint32 sampleRate,
                           quint32 blockSize){

    qDebug()<<"THREAD_SYNC_BEGIN";

    ShiftFinder f(sampleRate,blockSize);
    if(f.calcShiftInChannel(buffers,sampleRate)){
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
void SyncPairChannel::start(const ShPtrPacketBufferPair receiverStationClientPair,
                            quint32 ddcFrequency,
                            quint32 sampleRate,
                            quint32 blockSize)
{
    qDebug()<<"******SyncPairChannel::start();";
    qDebug()<<"COUNT"<<receiverStationClientPair.second.use_count();
    d->quit=false;
//    d->shiftFinder=new ShiftFinder();
//    d->shiftFinder->setBuffer1(d->syncBuffer1);
//    d->shiftFinder->setBuffer2(d->syncBuffer2);
//    d->shiftFinder->setSumDivBuffer(d->sumDivBuffer);
//    d->shiftFinder->start();
    d->fw.setFuture(QtConcurrent::run(this,&SyncPairChannel::sync,
                                      receiverStationClientPair,
                                      ddcFrequency,sampleRate,blockSize));
//    d->fw.setFuture(QtConcurrent::run(d->shiftFinder,&ShiftFinder::sync,
//                                      receiverStationClientPair,
//                                      ddcFrequency,sampleRate,blockSize));
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
//    d->shiftFinder->stop();
//    delete d->shiftFinder;
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
