#ifndef STREAM_H
#define STREAM_H

#include "channel_host.h"
#include "ring_buffer.h"
#include "receiver.pb.h"

#include <memory>

#include <QTcpServer>

#include <QFutureWatcher>
#include <QQueue>
#include <QMutex>
#include <QThread>

class CohG35DeviceSet;
class ConnectRequest;

class StreamDDC1:public QObject
{
    Q_OBJECT
public:
    StreamDDC1(net::ChannelHost*channelHost,
               std::shared_ptr<RingBuffer<proto::receiver::Packet>>buffer);

    ~StreamDDC1(){qDebug()<<"DESTR_DDC1";}
    void process();
    void start();
    void stop();
signals:
    void finished();
    void next();

private:
    std::shared_ptr<RingBuffer<proto::receiver::Packet>>buffer;
    net::ChannelHost *_streamSocket;
    std::atomic<bool>quit;
};


#endif // STREAM_H
