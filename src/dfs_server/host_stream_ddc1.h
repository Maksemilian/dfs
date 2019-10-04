#ifndef STREAM_H
#define STREAM_H

#include <QTcpServer>
#include <QFutureWatcher>
#include <QQueue>
#include <QMutex>

#include <memory>

class PeerWireClient;
class RingPacketBuffer;
class CohG35DeviceSet;
class ConnectRequest;

#include "channel_host.h"
#include "ring_buffer.h"
#include <QThread>

class StreamDDC1:public QObject
{
    Q_OBJECT
public:
    StreamDDC1(net::ChannelHost*channelHost,
               std::shared_ptr<RingBuffer>buffer);
    void process();
    void start();
    void stop();
    Q_SIGNAL void finished();
private:
    std::shared_ptr<RingBuffer>buffer;
    net::ChannelHost *_streamSocket;
    std::atomic<bool>quit;

};


#endif // STREAM_H
