#include "stream_server.h"

#include <QThread>

/*! \addtogroup client
 */

///@{

std::vector<ShPtrRadioChannel> StreamServer::getRadioChannels()
{
    std::vector<ShPtrRadioChannel> radioChannels;
    for (auto& it : _radioChannels)
    {
        radioChannels.push_back(it.second);
    }
    return radioChannels;
}

void StreamServer::createSession(net::ChannelHost* channelHost)
{
    if(channelHost->sessionType() == SessionType::SESSION_SIGNAL_STREAM)
    {
        qDebug() << "STREAM SESSION";
        _radioChannels[channelHost->peerAddress().toIPv4Address()] = std::make_shared<RadioChannel>();

        createThread(channelHost);
    }
    else
    {
        qDebug() << "ERROR SESSION TYPE";
    }
}

void StreamServer::createThread(net::ChannelHost* channelHost)
{
    QThread* thread = new QThread;

    StreamReader*   streamDDC1 =
        new StreamReader(channelHost,
                         _radioChannels[channelHost->peerAddress().toIPv4Address()]->inBuffer());
    streamDDC1->moveToThread(thread);
    channelHost->moveToThread(thread);

    connect(thread, &QThread::started,
            streamDDC1, &StreamReader::process);

    connect(streamDDC1, &StreamReader::finished,
            thread, &QThread::quit);

    connect(thread, &QThread::finished,
            streamDDC1, &StreamReader::deleteLater);

    connect(thread, &QThread::destroyed,
            thread, &QThread::deleteLater);

    thread->start();
}

struct StreamReader::Impl
{
    Impl(net::ChannelHost* channelHost,
         std::shared_ptr<RingBuffer<proto::receiver::Packet>>ddcBuffer):
        stream(channelHost),
        streamBuffer(ddcBuffer),
        quit(true)
    {
    }

    std::unique_ptr<net::ChannelHost> stream;
    std::shared_ptr<RingBuffer<proto::receiver::Packet>>streamBuffer;
    std::atomic<bool> quit;
    qint32 key;
};

StreamReader::StreamReader(net::ChannelHost* channelHost,
                           const ShPtrPacketBuffer streamBuffer)
    : d(std::make_unique<Impl>(channelHost, streamBuffer))
{

}

StreamReader::~StreamReader()
{
    qDebug() << "STOP Stream Reader DDC1";
    qDebug() << "DESTR::StreamReader";
}


void StreamReader::process()
{
    qDebug() << "START Stream Reader DDC1";
    connect(d->stream.get(), &net::ChannelHost::messageReceived,
            this, &StreamReader::onMessageReceive);

    connect(d->stream.get(), &net::ChannelHost::finished,
            this, &StreamReader::finished);
}

void StreamReader::onMessageReceive(const QByteArray& buffer)
{
    proto::receiver::ClientToHost clientToHost;

    if(!clientToHost.ParseFromArray(buffer.constData(), buffer.size()))
    {
        qDebug() << "ERROR PARSE STREAM_READER HOST TO CLIENT";
        return;
    }

    if(clientToHost.has_packet())
    {
        d->streamBuffer->push(const_cast<proto::receiver::Packet&>
                              (clientToHost.packet()));
        qDebug()
                << "BN:" << clientToHost.packet().block_number()
                << "SR:" << clientToHost.packet().sample_rate()
                << "TOW:" << clientToHost.packet().time_of_week()
                << "DDC_C:" << clientToHost.packet().ddc_sample_counter()
                << "ADC_C" << clientToHost.packet().adc_period_counter();
    }
}

///@}
