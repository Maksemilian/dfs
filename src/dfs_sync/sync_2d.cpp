#include "sync_2d.h"

#include "sync_block_equalizer.h"
#include "sync_calc_delta_pps.h"

#include "sync_test.h"

#include <QDebug>

//******************************* FindChannelForShift ******************************************

struct Sync2D::Impl
{
    //Impl() {}
    Impl(const ShPtrPacketBuffer& inBuffer1,
         const ShPtrPacketBuffer& inBuffer2,
         const SyncData& data):
        _data(data)
    {
        _pair1.first = inBuffer1;
        _pair1.second = std::make_shared<RingPacketBuffer>(4);

        _pair2.first = inBuffer2;
        _pair2.second = std::make_shared<RingPacketBuffer>(4);
    }

    std::pair<ShPtrPacketBuffer, ShPtrPacketBuffer>_pair1;
    std::pair<ShPtrPacketBuffer, ShPtrPacketBuffer>_pair2;

    SyncData _data;
    std::atomic_bool quit;
};

Sync2D::Sync2D(const ShPtrPacketBuffer& inBuffer1,
               const ShPtrPacketBuffer& inBuffer2,
               const SyncData& data):
    d(std::make_unique<Impl>(inBuffer1, inBuffer2, data)) {}

Sync2D::~Sync2D()
{
    qDebug() << "SYNC_PROCESS_DESTR";
};

bool Sync2D::readPacketFromBuffer(const ShPtrPacketBuffer& buffer,
                                  proto::receiver::Packet& packet,
                                  quint32 sampleRate)
{
    if(buffer->pop(packet) )
    {
        if(packet.sample_rate() == sampleRate)
        {
            return  true;
        }
    }
    return false;
}

void Sync2D::start()
{
    qDebug() << "THREAD_SYNC_BEGIN";
    double deltaPPS = 0.0;
    VectorIpp32fc shiftBuffer(d->_data.blockSize);

    if((deltaPPS = calcShiftAndShiftBuffer(shiftBuffer)) > 0)
    {
        BlockEqualizer blockEqualizer(shiftBuffer, d->_data,
                                      static_cast<quint32>(deltaPPS));
        SumSubMethod sumSubMethod(d->_data.sampleRate, d->_data.blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket = false;
        bool isSecondStationReadedPacket = false;

        qDebug() << "SHIFT_VALUE:" << deltaPPS
                 << "BUF_USE_COUNT" << d->_pair1.first.use_count() << d->_pair2.first.use_count();

        d->quit = false;
        while(!d->quit)
        {
            if(d->_pair1.first->pop(packet[CHANNEL_FIRST]))
            {
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket = true;
            }

            if(d->_pair2.first->pop(packet[CHANNEL_SECOND]))
            {
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket = true;
            }

            if(isFirstStationReadedPacket && isSecondStationReadedPacket)
            {
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST] = syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND] = syncQueuePair.second.dequeue();

                //TODO double deltaStart=1
                blockEqualizer.equateT(packet[CHANNEL_SECOND]);
                //                                qDebug()<<"BA_2";
                d->_pair1.second->push(packet[CHANNEL_FIRST]);
                d->_pair2.second->push(packet[CHANNEL_SECOND]);
                //

                //******* SUM-SUB METHOD **************
                sumSubMethod.apply(packet[CHANNEL_FIRST],
                                   packet[CHANNEL_SECOND],
                                   d->_data.blockSize);

                isFirstStationReadedPacket = false;
                isSecondStationReadedPacket = false;
            }
        }
    }
    else
    {
        qDebug("SYNC_ERROR");
    }

    qDebug() << "THREAD_SYNC_END";
    emit finished();
}

double Sync2D::calcShiftAndShiftBuffer(VectorIpp32fc& outShiftBuffer)
{
    qDebug() << "B_USE COUNT_calcShiftInChannel_BEGIN" << d->_pair1.first.use_count() << d->_pair2.first.use_count()
             << d->_data.sampleRate;
    Q_ASSERT_X(outShiftBuffer.size() == d->_data.blockSize, "Sync2D::calcShiftAndShiftBufferlTEST", "out_of_range");

    proto::receiver::Packet pct1;
    proto::receiver::Packet pct2;
    double deltaPPS = -1;
    // while(true){
    //WARNING В ТЕКУЩЕЙ ВЕРСИИ ПРЕДПОЛАГАЕТСЯ ЧТО
    // 1 канал стартовал раньше

    bool isPacket1Read = readPacketFromBuffer(d->_pair1.first, pct1, d->_data.sampleRate);

    bool isPacket2Read = readPacketFromBuffer(d->_pair2.first, pct2, d->_data.sampleRate);

    if(isPacket1Read && isPacket2Read)
    {
        if(pct1.time_of_week() !=  pct2.time_of_week())
        {
            qDebug() << "TOW NOT EQU:" << pct1.time_of_week() << pct2.time_of_week();

            isPacket1Read = false;
            isPacket2Read = false;
            //continue;//WARNING
        }

        CalcDeltaPPS c(pct1, pct2, d->_data.sampleRate,  d->_data.blockSize);

        if(pct1.time_of_week() == pct2.time_of_week())// 1 НОМЕРА СЕКУНД ОДИНАКОВЫЕ
        {
            deltaPPS = c.ddcAfterPPS1() - c.ddcAfterPPS2();
        }
        else // 2 НОМЕРА СЕКУНД РАЗНЫЕ
        {
            deltaPPS = c.deltaPPS();
        }

        cout << c << endl;
        outShiftBuffer = c.initShiftBuffer();
        return deltaPPS;
        //break;
    }
//}
    return deltaPPS;
}



void Sync2D::stop()
{
    d->quit = true;
}

//**************** PREVIOUS VARIANT **********************
/*
void Sync2D::initShiftBuffer(const float* signalData,
                             quint32 blockSize,
                             quint32 shift)
{
    qDebug() << "B_INIT_SHIFT_TEST" << shift << d->shiftData.shiftBuffer.size();
    for(quint32 i = 0; i < shift; i++)
    {
        d->shiftData.shiftBuffer[i].re = signalData[blockSize - (shift) + i * 2];
        d->shiftData.shiftBuffer[i].im = signalData[(blockSize - (shift) + i * 2) + 1];
    }
}
void Sync2D::showPacket(quint32 blockNumber,
                        quint32 sampleRate,
                        quint32 timeOfWeek,
                        double ddcSampleCounter,
                        quint64 adcPeriodCounter)
{
    qDebug() << "Packet" << blockNumber << sampleRate <<
             timeOfWeek << ddcSampleCounter << adcPeriodCounter;
}


double Sync2D::ddcAfterPPS(double ddcSampleCounter, quint32 blockNumber, quint32 blockSize)
{
    return (blockNumber * blockSize) - ddcSampleCounter;
}

void Sync2D::start()
{
    qDebug() << "THREAD_SYNC_BEGIN";
    d->quit = false;
    d->shiftData.channelIndex = -1;
    d->shiftData.deltaPPS = 0.0;
    d->shiftData.shiftBuffer.resize(d->_data.blockSize);

    if(calcShiftInChannel(d->_inBuffer1, d->_inBuffer2, d->_data.sampleRate))
    {
        BlockEqualizer blockEqualizer(d->shiftData.shiftBuffer,
                                      d->_data.blockSize);
        SumSubMethod sumSubMethod(d->_data.sampleRate, d->_data.blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket = false;
        bool isSecondStationReadedPacket = false;

        qDebug() << "CHANNEL_SHIFT:" << d->shiftData.channelIndex
                 << "SHIFT_VALUE:" << d->shiftData.deltaPPS
                 << "BUF_USE_COUNT" << d->_inBuffer1.use_count()
                 << d->_inBuffer2.use_count();

        while(!d->quit)
        {
            if(d->_inBuffer1->pop(packet[CHANNEL_FIRST]))
            {
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket = true;
            }

            if(d->_inBuffer2->pop(packet[CHANNEL_SECOND]))
            {
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket = true;
            }

            if(isFirstStationReadedPacket && isSecondStationReadedPacket)
            {
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST] = syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND] = syncQueuePair.second.dequeue();

                Ipp32fc* signal = reinterpret_cast<Ipp32fc*>
                                  (const_cast<float*>
                                   (packet[d->shiftData.channelIndex].sample().data()));

                blockEqualizer.equate(signal,
                                      d->_data.blockSize,
                                      d->shiftData.deltaPPS,
                                      d->_data.ddcFrequency,
                                      d->_data.sampleRate, d->shiftData.deltaStart);
                //                                qDebug()<<"BA_2";
                d->_outBuffer1->push(packet[CHANNEL_FIRST]);
                d->_outBuffer2->push(packet[CHANNEL_SECOND]);
                //


                const Ipp32fc* dst1 = reinterpret_cast<const Ipp32fc*>
                                      (packet[CHANNEL_FIRST].sample().data());

                const Ipp32fc* dst2 = reinterpret_cast<const Ipp32fc*>
                                      (packet[CHANNEL_SECOND].sample().data());

                sumSubMethod.buffer()->push(sumSubMethod.
                                            calc(dst1, dst2,
                                                 d->_data.blockSize));

                isFirstStationReadedPacket = false;
                isSecondStationReadedPacket = false;
            }
        }
    }
    else
    {
        qDebug("SYNC_ERROR");
    }

    qDebug() << "THREAD_SYNC_END";
    emit finished();
}


bool Sync2D::calcShiftInChannel(const ShPtrPacketBuffer& _inBuffer1,
                                const ShPtrPacketBuffer& inBuffer2,
                                quint32 sampleRate)
{
    qDebug() << "B_USE COUNT_calcShiftInChannel_BEGIN"
             << _inBuffer1.use_count()
             << inBuffer2.use_count()
             << sampleRate
             //<< d->shiftData.shiftBuffer.size()
                ;

    BoolPair isReadedPacketPair;

    isReadedPacketPair.first = false;
    isReadedPacketPair.second = false;

    bool shiftFound = false;
    proto::receiver::Packet packetPair[2];

    proto::receiver::Packet packet;
    while(!shiftFound)
    {
        //                qDebug()<<"read_1";
        if(_inBuffer1->pop(packet))
        {
            if(packet.sample_rate() == sampleRate)
            {
                packetPair[CHANNEL_FIRST] = packet;
                isReadedPacketPair.first = true;
                qDebug() << "read_1" << packet.block_number();
            }
        }
        //                qDebug()<<"read_2";
        if(inBuffer2->pop(packet))
        {
            if(packet.sample_rate() == sampleRate)
            {
                packetPair[CHANNEL_SECOND] = packet;
                isReadedPacketPair.second = true;
                qDebug() << "read_2" << packet.block_number();
            }
        }

        if(isReadedPacketPair.first && isReadedPacketPair.second)
        {
            if(packetPair[CHANNEL_FIRST].time_of_week() !=
                    packetPair[CHANNEL_SECOND].time_of_week())
            {
                qDebug() << "BAD EQUALS:"
                         << packetPair[CHANNEL_FIRST].time_of_week()
                         << packetPair[CHANNEL_SECOND].time_of_week();

                isReadedPacketPair.first = false;
                isReadedPacketPair.second = false;
                continue;
            }

            Q_ASSERT_X(packetPair[CHANNEL_FIRST].time_of_week() ==
                       packetPair[CHANNEL_SECOND].time_of_week(),
                       "SyncPairChannel::calcDdcDifferenceBetweenCannel", "time of week not equ");

            double ddcDelayAfterLastPpsFirst = ddcAfterPPS(packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                                               packetPair[CHANNEL_FIRST].block_number(),
                                               packetPair[CHANNEL_FIRST].block_size());

            double ddcDelayAfterLastPpsSecond = ddcAfterPPS(packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                                                packetPair[CHANNEL_SECOND].block_number(),
                                                packetPair[CHANNEL_SECOND].block_size());
            // 100 200
            d->shiftData.deltaPPS =  ddcDelayAfterLastPpsFirst - ddcDelayAfterLastPpsSecond;

            d->shiftData.channelIndex = d->shiftData.deltaPPS < 0 ?
                                        CHANNEL_SECOND   :  CHANNEL_FIRST;
            d->shiftData.deltaPPS = abs(d->shiftData.deltaPPS);

            //TODO поместить в один метод
            //BN1*BS1+SHIFT-BN2*BS2
            d->shiftData.deltaStart = d->shiftData.channelIndex == CHANNEL_FIRST ?
                                      ((packetPair[CHANNEL_FIRST].block_number() *
                                        packetPair[CHANNEL_FIRST].block_size()) + d->shiftData.deltaPPS)
                                      - packetPair[CHANNEL_SECOND].block_number() * packetPair[CHANNEL_SECOND].block_size()
                                      :
                                      ((packetPair[CHANNEL_SECOND].block_number() *
                                        packetPair[CHANNEL_SECOND].block_size()) + d->shiftData.deltaPPS)
                                      - packetPair[CHANNEL_FIRST].block_number() * packetPair[CHANNEL_FIRST].block_size()
                                      ;
            d->shiftData.deltaStart = abs(d->shiftData.deltaStart);

            Q_ASSERT_X( d->shiftData.deltaPPS >= 0 &&  d->shiftData.deltaPPS < packet.block_size(),
                        "SignalSync::sync", "error shift");

            if(static_cast<quint32>(d->shiftData.deltaPPS) >
                    packetPair[d->shiftData.channelIndex].block_size())
            {
                qDebug() << "******************BAD SHIFT SIZE"
                         << d->shiftData.deltaPPS
                         << d->shiftData.deltaStart;
                isReadedPacketPair.first = false;
                isReadedPacketPair.second = false;
                continue;
            }

            initShiftBuffer(packetPair[d->shiftData.channelIndex].sample().data(),
                            packetPair[d->shiftData.channelIndex].block_size(),
                            static_cast<quint32>(d->shiftData.deltaPPS));

            //            initShiftBuffer(packetPair[d->channelIndex].sample().data(),
            //                    static_cast<quint32>(packetPair[d->channelIndex].sample().size()),
            //                    static_cast<quint32>(d->ddcDifference)*COUNT_SIGNAL_COMPONENT);

            showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
                       packetPair[CHANNEL_FIRST].time_of_week(), packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                       packetPair[CHANNEL_FIRST].adc_period_counter());

            showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
                       packetPair[CHANNEL_SECOND].time_of_week(), packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                       packetPair[CHANNEL_SECOND].adc_period_counter());

            qDebug() << "*********SHIFT_FOUND" << d->shiftData.channelIndex
                     << d->shiftData.deltaPPS
                     << d->shiftData.deltaStart;
            shiftFound = true;
        }
    }

    return shiftFound;
}
*/
//************************** DEL ***************
/*
void SyncProcess::start(const ShPtrPacketBufferPair buffers,
                        quint32 ddcFrequency,
                        quint32 sampleRate,
                        quint32 blockSize)
{

    qDebug() << "THREAD_SYNC_BEGIN";
    d->quit = false;
    d->shiftData.channelIndex = -1;
    d->shiftData.ddcDifference = 0.0;
    d->shiftData.shiftBuffer.resize(blockSize);

    if(calcShiftInChannel(buffers, sampleRate))
    {
        BlockEqualizer blockEqualizer(d->shiftData.shiftBuffer, blockSize);
        SumSubMethod sumSubMethod(sampleRate, blockSize);

        proto::receiver::Packet packet[CHANNEL_SIZE];
        PacketQueuePair syncQueuePair;

        bool isFirstStationReadedPacket = false;
        bool isSecondStationReadedPacket = false;

        qDebug() << "CHANNEL_SHIFT:" << d->shiftData.channelIndex
                 << "SHIFT_VALUE:" << d->shiftData.ddcDifference
                 << "BUF_USE_COUNT" << buffers.first.use_count() << buffers.second.use_count();

        while(!d->quit)
        {
            if(buffers.first->pop(packet[CHANNEL_FIRST]))
            {
                syncQueuePair.first.enqueue(packet[CHANNEL_FIRST]);
                isFirstStationReadedPacket = true;
            }

            if(buffers.second->pop(packet[CHANNEL_SECOND]))
            {
                syncQueuePair.second.enqueue(packet[CHANNEL_SECOND]);
                isSecondStationReadedPacket = true;
            }

            if(isFirstStationReadedPacket && isSecondStationReadedPacket)
            {
                //                qDebug()<<"Is find_B"<<syncQueuePair.first.size()<<syncQueuePair.second.size();
                packet[CHANNEL_FIRST] = syncQueuePair.first.dequeue();
                packet[CHANNEL_SECOND] = syncQueuePair.second.dequeue();

                Ipp32fc* signal = reinterpret_cast<Ipp32fc*>
                                  (const_cast<float*>
                                   (packet[d->shiftData.channelIndex].sample().data()));

                blockEqualizer.equate(signal, blockSize, d->shiftData.ddcDifference,
                                      ddcFrequency, sampleRate, d->shiftData.deltaStart);
                //                                qDebug()<<"BA_2";
                d->syncBuffer1->push(packet[CHANNEL_FIRST]);
                d->syncBuffer2->push(packet[CHANNEL_SECOND]);
                //

                //
                const Ipp32fc* dst1 = reinterpret_cast<const Ipp32fc*>
                                      (packet[CHANNEL_FIRST].sample().data());

                const Ipp32fc* dst2 = reinterpret_cast<const Ipp32fc*>
                                      (packet[CHANNEL_SECOND].sample().data());

                d->sumDivBuffer->push(sumSubMethod.
                                      calc(dst1, dst2, blockSize));

                isFirstStationReadedPacket = false;
                isSecondStationReadedPacket = false;
            }
        }
    }
    else
    {
        qDebug("SYNC_ERROR");
    }

    qDebug() << "THREAD_SYNC_END";
    emit finished();
}
bool SyncProcess::calcShiftInChannel(const ShPtrPacketBufferPair stationPair,
                                     quint32 sampleRate)
{
    qDebug() << "B_USE COUNT_calcShiftInChannel_BEGIN"
             << stationPair.first.use_count()
             << stationPair.first.use_count()
             << sampleRate
             << d->shiftData.shiftBuffer.size();

    BoolPair isReadedPacketPair;

    isReadedPacketPair.first = false;
    isReadedPacketPair.second = false;

    bool shiftFound = false;
    proto::receiver::Packet packetPair[2];

    proto::receiver::Packet packet;
    while(!shiftFound)
    {
        //                qDebug()<<"read_1";
        if(stationPair.first->pop(packet))
        {
            if(packet.sample_rate() == sampleRate)
            {
                packetPair[CHANNEL_FIRST] = packet;
                isReadedPacketPair.first = true;
                qDebug() << "read_1" << packet.block_number();
            }
        }
        //                qDebug()<<"read_2";
        if(stationPair.second->pop(packet))
        {
            if(packet.sample_rate() == sampleRate)
            {
                packetPair[CHANNEL_SECOND] = packet;
                isReadedPacketPair.second = true;
                qDebug() << "read_2" << packet.block_number();
            }
        }

        if(isReadedPacketPair.first && isReadedPacketPair.second)
        {
            if(packetPair[CHANNEL_FIRST].time_of_week() !=
                    packetPair[CHANNEL_SECOND].time_of_week())
            {
                qDebug() << "BAD EQUALS:"
                         << packetPair[CHANNEL_FIRST].time_of_week()
                         << packetPair[CHANNEL_SECOND].time_of_week();

                isReadedPacketPair.first = false;
                isReadedPacketPair.second = false;
                continue;
            }

            Q_ASSERT_X(packetPair[CHANNEL_FIRST].time_of_week() ==
                       packetPair[CHANNEL_SECOND].time_of_week(),
                       "SyncPairChannel::calcDdcDifferenceBetweenCannel", "time of week not equ");

            double ddcDelayAfterLastPpsFirst = ddcAfterLastPps(packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                                               packetPair[CHANNEL_FIRST].block_number(),
                                               packetPair[CHANNEL_FIRST].block_size());

            double ddcDelayAfterLastPpsSecond = ddcAfterLastPps(packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                                                packetPair[CHANNEL_SECOND].block_number(),
                                                packetPair[CHANNEL_SECOND].block_size());
// 100 200
            d->shiftData.ddcDifference =  ddcDelayAfterLastPpsFirst - ddcDelayAfterLastPpsSecond;

            d->shiftData.channelIndex = d->shiftData.ddcDifference < 0 ?
                                        CHANNEL_SECOND   :  CHANNEL_FIRST;
            d->shiftData.ddcDifference = abs(d->shiftData.ddcDifference);

            //TODO поместить в один метод
            //BN1*BS1+SHIFT-BN2*BS2
            d->shiftData.deltaStart = d->shiftData.channelIndex == CHANNEL_FIRST ?
                                      ((packetPair[CHANNEL_FIRST].block_number() *
                                        packetPair[CHANNEL_FIRST].block_size()) + d->shiftData.ddcDifference)
                                      - packetPair[CHANNEL_SECOND].block_number() * packetPair[CHANNEL_SECOND].block_size()
                                      :
                                      ((packetPair[CHANNEL_SECOND].block_number() *
                                        packetPair[CHANNEL_SECOND].block_size()) + d->shiftData.ddcDifference)
                                      - packetPair[CHANNEL_FIRST].block_number() * packetPair[CHANNEL_FIRST].block_size()
                                      ;
            d->shiftData.deltaStart = abs(d->shiftData.deltaStart);

            Q_ASSERT_X( d->shiftData.ddcDifference >= 0 &&  d->shiftData.ddcDifference < packet.block_size(),
                        "SignalSync::sync", "error shift");

            if(static_cast<quint32>(d->shiftData.ddcDifference) >
                    packetPair[d->shiftData.channelIndex].block_size())
            {
                qDebug() << "******************BAD SHIFT SIZE"
                         << d->shiftData.ddcDifference
                         << d->shiftData.deltaStart;
                isReadedPacketPair.first = false;
                isReadedPacketPair.second = false;
                continue;
            }

            initShiftBuffer(packetPair[d->shiftData.channelIndex].sample().data(),
                            packetPair[d->shiftData.channelIndex].block_size(),
                            static_cast<quint32>(d->shiftData.ddcDifference));

            //            initShiftBuffer(packetPair[d->channelIndex].sample().data(),
            //                    static_cast<quint32>(packetPair[d->channelIndex].sample().size()),
            //                    static_cast<quint32>(d->ddcDifference)*COUNT_SIGNAL_COMPONENT);

            showPacket(packetPair[CHANNEL_FIRST].block_number(), packetPair[CHANNEL_FIRST].sample_rate(),
                       packetPair[CHANNEL_FIRST].time_of_week(), packetPair[CHANNEL_FIRST].ddc_sample_counter(),
                       packetPair[CHANNEL_FIRST].adc_period_counter());

            showPacket(packetPair[CHANNEL_SECOND].block_number(), packetPair[CHANNEL_SECOND].sample_rate(),
                       packetPair[CHANNEL_SECOND].time_of_week(), packetPair[CHANNEL_SECOND].ddc_sample_counter(),
                       packetPair[CHANNEL_SECOND].adc_period_counter());

            qDebug() << "*********SHIFT_FOUND" << d->shiftData.channelIndex
                     << d->shiftData.ddcDifference
                     << d->shiftData.deltaStart;
            shiftFound = true;
        }
    }

    return shiftFound;
}
*/
