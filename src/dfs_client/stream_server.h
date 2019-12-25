#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include <channel_host.h>
#include <ring_buffer.h>
#include "receiver.pb.h"
//#include <client_ds_stream.h>
#include <QTcpServer>
#include <QList>



using ShPtrPacketBuffer =std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class ClStreamReaderT:public QObject
{
    Q_OBJECT
public:
    ClStreamReaderT(net::ChannelHost *channelHost,
                  const ShPtrPacketBuffer streamBuffer);
    ~ClStreamReaderT();
    void process();
//    void start();
//    void stop();
signals:
//    void started();
//    void stoped();
    void finished();
private:
    void onMessageReceive(const QByteArray&buffer);
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};


class ClStreamServer:public QTcpServer
{
    Q_OBJECT
public:
    enum class StreamType{ST_DDC1};
    ClStreamServer(quint8 bufferSize=16);
    ~ClStreamServer()override;
    ShPtrPacketBuffer getBuffer(StreamType type);
signals:
    void newChannelReady();
private:
    void incomingConnection(qintptr handle) override;
    void onChannelReady();
    void onNewConnection();
    void onChannelDisconnected();
    void createSession(net::ChannelHost*channelHost);
    void createThread(net::ChannelHost *channelHost);
private:
    QList<net::ChannelHost*>_pendingChannelsList;
    QList<net::ChannelHost*>_readyChannelsList;
    std::map<StreamType,ShPtrPacketBuffer> buffers;
};


#endif // STREAM_SERVER_H
