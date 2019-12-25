#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include <channel_host.h>
#include <ring_buffer.h>
#include "receiver.pb.h"
//#include <client_ds_stream.h>
#include <QTcpServer>
#include <QList>

class StreamReaderT;
using ShPtrPacketBuffer =std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class StreamServer:public QTcpServer
{
public:
    enum class StreamType{ST_DDC1};
    StreamServer(quint8 bufferSize=16);
    ~StreamServer()override;
signals:
    void newChannelReady();
private:
    void incomingConnection(qintptr handle) override;
    void onChannelReady();
    void onNewConnection();
    void onChannelDisconnected();
    void createSession(net::ChannelHost*channelHost);
    void createThread(net::ChannelHost *channelHost);
    ShPtrPacketBuffer getBuffer(StreamType type);
private:
    QList<net::ChannelHost*>_pendingChannelsList;
    QList<net::ChannelHost*>_readyChannelsList;
    QList<StreamReaderT*>_streamReaders;
    std::map<StreamType,ShPtrPacketBuffer> buffers;
};

#endif // STREAM_SERVER_H
