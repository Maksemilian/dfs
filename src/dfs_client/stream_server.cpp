#include "stream_server.h"
//#include "client_ds_stream.h"
#include "channel_host.h"
#include "ring_buffer.h"
#include "receiver.pb.h"

#include <QThread>
using namespace  std;
class StreamReaderT:public QObject
{
    Q_OBJECT
public:
    StreamReaderT(net::ChannelHost *channelHost,
                  const std::shared_ptr<RingBuffer<proto::receiver::Packet>>streamBuffer);
    ~StreamReaderT();
    void start();
    void stop();
    qint32 key();
signals:
    void started();
    void stoped();
    void finished();
private:
    void run();
    void onMessageReceive(const QByteArray&buffer);
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};

struct StreamReaderT::Impl
{
    Impl(net::ChannelHost*channelHost,
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

StreamReaderT::StreamReaderT(net::ChannelHost *channelHost,
                             const std::shared_ptr<RingBuffer<proto::receiver::Packet>> streamBuffer)
    :d(std::make_unique<Impl>(channelHost,streamBuffer))
{

}

StreamReaderT::~StreamReaderT(){qDebug()<<"DESTR::StreamReader";}

qint32 StreamReaderT::key()
{
    return d->key;
}

void StreamReaderT::start()
{
    if(d->quit){
        d->quit=false;
        run();
        emit started();
        qDebug("START STREAM READER");
    }
}

void StreamReaderT::stop()
{
    if(!d->quit){
        d->quit=true;
        disconnect(d->stream.get(),&net::ChannelHost::messageReceived,
                   this,&StreamReaderT::onMessageReceive);
        emit stoped();
        qDebug("STOP STREAM READER");
    }
}

void StreamReaderT::run()
{
    //    d->stream.reset(new net::ChannelHost);
    //    d->stream->connectToHost(d->address,d->port,SessionType::SESSION_SIGNAL_STREAM);
    connect(d->stream.get(),&net::ChannelHost::messageReceived,
            this,&StreamReaderT::onMessageReceive);

    connect(d->stream.get(),&net::ChannelHost::finished,
            this,&StreamReaderT::finished);

}

void StreamReaderT::onMessageReceive(const QByteArray &buffer)
{
    //    qDebug()<<"StreamReader::onMessageReceive"<<buffer.size()<<d->quit;
    proto::receiver::ClientToHost clientToHost;

    if(!clientToHost.ParseFromArray(buffer.constData(),buffer.size())){
        qDebug()<<"ERROR PARSE STREAM_READER HOST TO CLIENT";
        return;
    }

    if(clientToHost.has_packet()){
        d->streamBuffer->push(const_cast<proto::receiver::Packet&>(clientToHost.packet()));
        qDebug()
                <<"BN:"<<clientToHost.packet().block_number()
               <<"SR:"<<clientToHost.packet().sample_rate()
              <<"TOW:"<<clientToHost.packet().time_of_week()
             <<"DDC_C:"<<clientToHost.packet().ddc_sample_counter()
            <<"ADC_C"<<clientToHost.packet().adc_period_counter();
    }
}

//******************************

StreamServer::StreamServer(quint8 bufferSize)
{
    qDebug()<<"Stream Server Init";
    connect(this,&StreamServer::newChannelReady,
            this,&StreamServer::onNewConnection);

    buffers[StreamType::ST_DDC1]=std::make_shared<RingBuffer<proto::receiver::Packet>>(bufferSize);
}

ShPtrPacketBuffer StreamServer::getBuffer(StreamType type)
{
    if(type==StreamType::ST_DDC1){
        return buffers[StreamType::ST_DDC1];
    }
    return nullptr;
}
void StreamServer::incomingConnection(qintptr handle)
{
    qDebug()<<"===============incomingConnection handle"<<handle;
    net::ChannelHost *channelHost=new net::ChannelHost(handle);

    connect(channelHost,&net::ChannelHost::keyExchangedFinished,
            this,&StreamServer::onChannelReady);

    connect(channelHost,&net::ChannelHost::finished,
            this,&StreamServer::onChannelDisconnected);

    _pendingChannelsList.append(channelHost);
}

void StreamServer::onChannelReady()
{
    qDebug()<<"Server::onChannelReady";

    auto it=_pendingChannelsList.begin();
    while (it!=_pendingChannelsList.end()) {
        net::ChannelHost* networkChannel=*it;

        if(!networkChannel)
        {
            it =_pendingChannelsList.erase(it);
        }
        else if(networkChannel->state()==net::ChannelHost::ESTABLISHED)
        {
            it =_pendingChannelsList.erase(it);
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

void StreamServer::onNewConnection()
{
    qDebug()<<"Server::onNewConnection";

    while (!_readyChannelsList.isEmpty()) {
        net::ChannelHost* networkChannel=_readyChannelsList.front();
        qDebug()<<networkChannel->sessionType();
        if(networkChannel){
            qDebug()<<"Create Session";
            createSession(networkChannel);
            _readyChannelsList.pop_front();
        }else {
            qDebug()<<"NULLPTR SESSION TYPE";
        }
    }
}

void StreamServer::onChannelDisconnected()
{
    qDebug()<<"onChannelDisconnected";
//    _client.first()->deleteLater();
}

void StreamServer::createSession(net::ChannelHost *channelHost)
{
    if(channelHost->sessionType()==SessionType::SESSION_SIGNAL_STREAM){
        qDebug()<<"STREAM SESSION";
        createThread(channelHost);
    }else qDebug()<<"ERROR SESSION TYPE";
}

void StreamServer::createThread(net::ChannelHost *channelHost)
{
    QThread *thread=new QThread;

    StreamReaderT*   _streamDDC1=new StreamReaderT(channelHost,buffers[StreamType::ST_DDC1]);
    _streamDDC1->moveToThread(thread);
    channelHost->moveToThread(thread);
    //        connect(channelHost,&net::ChannelHost::finished,
    //                [this]{
    //           _streamDDC1->stop();
    //        });
    connect(thread,&QThread::started,
            _streamDDC1,&StreamReaderT::start);

    connect(_streamDDC1,&StreamReaderT::finished,
            thread,&QThread::quit);

    connect(thread,&QThread::finished,
            _streamDDC1,&StreamReaderT::deleteLater);

    connect(thread,&QThread::destroyed,
            thread,&QThread::deleteLater);

    thread->start();
}

StreamServer::~StreamServer()
{
}
