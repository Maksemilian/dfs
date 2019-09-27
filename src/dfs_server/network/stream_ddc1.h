#ifndef STREAM_H
#define STREAM_H
#include <QTcpServer>
#include <QFutureWatcher>
#include <memory>
#include <QQueue>
#include <QMutex>

class PeerWireClient;
class RingPacketBuffer;
class CohG35DeviceSet;
class ConnectRequest;
class StreamServer;
class StreamFile;
class StreamDDC1:public QObject
{
    Q_OBJECT
public:
    StreamDDC1(PeerWireClient*, std::shared_ptr<RingPacketBuffer>buffer);
    void process();
    void start();
    void stop();
    Q_SIGNAL void finished();
private:
    std::shared_ptr<RingPacketBuffer>buffer;
    PeerWireClient *streamSocket;
    std::atomic<bool>quit;
};


#endif // STREAM_H