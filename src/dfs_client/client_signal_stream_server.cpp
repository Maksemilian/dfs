#include "client_signal_stream_server.h"
#include "channel_host.h"

#include "ring_buffer.h"
#include "receiver.pb.h"

#include <QThread>

SignalStreamServer::SignalStreamServer(quint8 bufferSize)
{
    qDebug() << "Stream Server Init";
    connect(this, &SignalStreamServer::newChannelReady,
            this, &SignalStreamServer::onNewConnection);

    buffers[StreamType::ST_DDC1] = std::make_shared<RingBuffer<proto::receiver::Packet>>(bufferSize);
}

ShPtrPacketBuffer SignalStreamServer::getBuffer(StreamType type)
{
    if(type == StreamType::ST_DDC1)
    {
        return buffers[StreamType::ST_DDC1];
    }
    return nullptr;
}
void SignalStreamServer::incomingConnection(qintptr handle)
{
    qDebug() << "=============== Stream_Serever_incomingConnection handle" << handle;
    net::ChannelHost* channelHost = new net::ChannelHost(handle);

    connect(channelHost, &net::ChannelHost::keyExchangedFinished,
            this, &SignalStreamServer::onChannelReady);

    _pendingChannelsList.append(channelHost);
}

void SignalStreamServer::onChannelReady()
{
    qDebug() << "Server::onChannelReady";

    auto it = _pendingChannelsList.begin();
    while (it != _pendingChannelsList.end())
    {
        net::ChannelHost* networkChannel = *it;

        if(!networkChannel)
        {
            it = _pendingChannelsList.erase(it);
        }
        else if(networkChannel->state() == net::ChannelHost::ESTABLISHED)
        {
            it = _pendingChannelsList.erase(it);
            //            qDebug()<<networkChannel<<*it;
            _readyChannelsList.append(networkChannel);
            emit newChannelReady();
        }
        else
        {
            it++;
        }
    }
}

void SignalStreamServer::onNewConnection()
{
    qDebug() << "Server::onNewConnection";

    while (!_readyChannelsList.isEmpty())
    {
        net::ChannelHost* networkChannel = _readyChannelsList.front();
        qDebug() << networkChannel->sessionType();
        if(networkChannel)
        {
            qDebug() << "Create Session";
            createSession(networkChannel);
            _readyChannelsList.pop_front();
        }
        else
        {
            qDebug() << "NULLPTR SESSION TYPE";
        }
    }
}

void SignalStreamServer::createSession(net::ChannelHost* channelHost)
{
    if(channelHost->sessionType() == SessionType::SESSION_SIGNAL_STREAM)
    {
        qDebug() << "STREAM SESSION";
        createThread(channelHost);
    }
    else qDebug() << "ERROR SESSION TYPE";
}

void SignalStreamServer::createThread(net::ChannelHost* channelHost)
{
    QThread* thread = new QThread;

    SignalStreamReader*   streamDDC1 = new SignalStreamReader(channelHost, buffers[StreamType::ST_DDC1]);
    streamDDC1->moveToThread(thread);
    channelHost->moveToThread(thread);

    connect(thread, &QThread::started,
            streamDDC1, &SignalStreamReader::process);

    connect(streamDDC1, &SignalStreamReader::finished,
            thread, &QThread::quit);

    connect(thread, &QThread::finished,
            streamDDC1, &SignalStreamReader::deleteLater);

    connect(thread, &QThread::destroyed,
            thread, &QThread::deleteLater);

    thread->start();
}

//************************** SIGNAL STREM READER **************************

struct SignalStreamReader::Impl
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

SignalStreamReader::SignalStreamReader(net::ChannelHost* channelHost,
                                       const ShPtrPacketBuffer streamBuffer)
    : d(std::make_unique<Impl>(channelHost, streamBuffer))
{

}

SignalStreamReader::~SignalStreamReader()
{
    qDebug() << "STOP Stream Reader DDC1";
    qDebug() << "DESTR::StreamReader";
}


void SignalStreamReader::process()
{
    qDebug() << "START Stream Reader DDC1";
    connect(d->stream.get(), &net::ChannelHost::messageReceived,
            this, &SignalStreamReader::onMessageReceive);

    connect(d->stream.get(), &net::ChannelHost::finished,
            this, &SignalStreamReader::finished);
}

void SignalStreamReader::onMessageReceive(const QByteArray& buffer)
{
    proto::receiver::ClientToHost clientToHost;

    if(!clientToHost.ParseFromArray(buffer.constData(), buffer.size()))
    {
        qDebug() << "ERROR PARSE STREAM_READER HOST TO CLIENT";
        return;
    }

    if(clientToHost.has_packet())
    {
        d->streamBuffer->push(const_cast<proto::receiver::Packet&>(clientToHost.packet()));
        qDebug()
                << "BN:" << clientToHost.packet().block_number()
                << "SR:" << clientToHost.packet().sample_rate()
                << "TOW:" << clientToHost.packet().time_of_week()
                << "DDC_C:" << clientToHost.packet().ddc_sample_counter()
                << "ADC_C" << clientToHost.packet().adc_period_counter();
    }
}
