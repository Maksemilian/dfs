#ifndef STREAM_H
#define STREAM_H

#include <QTcpServer>
#include <QFutureWatcher>
#include <QQueue>
#include <QMutex>

#include <memory>

class CohG35DeviceSet;
class ConnectRequest;

#include "channel_host.h"
#include "ring_buffer.h"
#include <QThread>
#include "receiver.pb.h"
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
