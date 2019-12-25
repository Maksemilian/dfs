#include "stream_server.h"
//#include "client_ds_stream.h"
#include "channel_host.h"
#include "ring_buffer.h"
#include "receiver.pb.h"

#include <QThread>
using namespace  std;

//class StreamReaderT:public QObject
//{
//    Q_OBJECT
//public:
//    StreamReaderT(net::ChannelHost *channelHost,
//                  const std::shared_ptr<RingBuffer<proto::receiver::Packet>>streamBuffer);
//    ~StreamReaderT();
//    void start();
//    void stop();
//    qint32 key();
//signals:
//    void started();
//    void stoped();
//    void finished();
//private:
//    void run();
//    void onMessageReceive(const QByteArray&buffer);
//private:
//    struct Impl;
//    std::unique_ptr<Impl>d;
//};

struct ClStreamReaderT::Impl
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

ClStreamReaderT::ClStreamReaderT(net::ChannelHost *channelHost,
                             const ShPtrPacketBuffer streamBuffer)
    :d(std::make_unique<Impl>(channelHost,streamBuffer))
{

}

ClStreamReaderT::~ClStreamReaderT(){
    qDebug()<<"STOP Stream Reader DDC1";
    qDebug()<<"DESTR::StreamReader";
}

//void ClStreamReaderT::start()
//{
//    if(d->quit){
//        d->quit=false;
//        process();
//        emit started();
//        qDebug("START STREAM READER");
//    }
//}

//void ClStreamReaderT::stop()
//{
//    if(!d->quit){
//        d->quit=true;
//        disconnect(d->stream.get(),&net::ChannelHost::messageReceived,
//                   this,&ClStreamReaderT::onMessageReceive);
//        emit stoped();
//        qDebug("STOP STREAM READER");
//    }
//}

void ClStreamReaderT::process()
{
    qDebug()<<"START Stream Reader DDC1";
    connect(d->stream.get(),&net::ChannelHost::messageReceived,
            this,&ClStreamReaderT::onMessageReceive);

    connect(d->stream.get(),&net::ChannelHost::finished,
            this,&ClStreamReaderT::finished);
}

void ClStreamReaderT::onMessageReceive(const QByteArray &buffer)
{
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

ClStreamServer::ClStreamServer(quint8 bufferSize)
{
    qDebug()<<"Stream Server Init";
    connect(this,&ClStreamServer::newChannelReady,
            this,&ClStreamServer::onNewConnection);

    buffers[StreamType::ST_DDC1]=std::make_shared<RingBuffer<proto::receiver::Packet>>(bufferSize);
}

ShPtrPacketBuffer ClStreamServer::getBuffer(StreamType type)
{
    if(type==StreamType::ST_DDC1){
        return buffers[StreamType::ST_DDC1];
    }
    return nullptr;
}
void ClStreamServer::incomingConnection(qintptr handle)
{
    qDebug()<<"=============== Stream_Serever_incomingConnection handle"<<handle;
    net::ChannelHost *channelHost=new net::ChannelHost(handle);

    connect(channelHost,&net::ChannelHost::keyExchangedFinished,
            this,&ClStreamServer::onChannelReady);

//    connect(channelHost,&net::ChannelHost::finished,
//            this,&ClStreamServer::onChannelDisconnected);

    _pendingChannelsList.append(channelHost);
}

void ClStreamServer::onChannelReady()
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

void ClStreamServer::onNewConnection()
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

void ClStreamServer::onChannelDisconnected()
{
    qDebug()<<"onChannelDisconnected";

//    _client.first()->deleteLater();
}

void ClStreamServer::createSession(net::ChannelHost *channelHost)
{
    if(channelHost->sessionType()==SessionType::SESSION_SIGNAL_STREAM){
        qDebug()<<"STREAM SESSION";
        createThread(channelHost);
    }else qDebug()<<"ERROR SESSION TYPE";
}

void ClStreamServer::createThread(net::ChannelHost *channelHost)
{
    QThread *thread=new QThread;

    ClStreamReaderT*   streamDDC1=new ClStreamReaderT(channelHost,buffers[StreamType::ST_DDC1]);
    streamDDC1->moveToThread(thread);
    channelHost->moveToThread(thread);

    connect(thread,&QThread::started,
            streamDDC1,&ClStreamReaderT::process);

    connect(streamDDC1,&ClStreamReaderT::finished,
            thread,&QThread::quit);

    connect(thread,&QThread::finished,
            streamDDC1,&ClStreamReaderT::deleteLater);

    connect(thread,&QThread::destroyed,
            thread,&QThread::deleteLater);

    thread->start();
}

ClStreamServer::~ClStreamServer()
{
}
