#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H

#include <memory>

#include <QTcpServer>
#include <QFutureWatcher>
#include <QQueue>
#include <QMutex>

class PeerWireClient;
class RingPacketBuffer;
class CohG35DeviceSet;
class ConnectRequest;
class StreamServer;

class StreamFile;
class StreamDDC1;

class StreamAnalizator :public QObject{
    Q_OBJECT

    static const int TIME_FOR_READY_READ_MS=1000;
public:
    StreamAnalizator(StreamServer*server);
    void start();
    void stop();
    void pushSocketSescriptor(qintptr socketDescriptor);
    bool popSocketDescriptor(qintptr &socetDescriptor);
private:
    void run();
    ConnectRequest readStreamType(PeerWireClient *streamSocket);
    void sendAnswer(PeerWireClient *streamSocket,bool state);
    void createStreamDDC1(PeerWireClient *streamSocket);
    void createFileStream(PeerWireClient *streamSocket);
private:
    StreamServer*server;
    QMutex mutex;
    QQueue<qintptr>desckriptorQueue;
    QFutureWatcher<void>fw;
    std::atomic<bool>quit;
};

class StreamServer:public QTcpServer
{
    Q_OBJECT
    friend class StreamAnalizator;


public:
    StreamServer(std::shared_ptr<CohG35DeviceSet>deviceSet);
    ~StreamServer();

    void stopStreamDDC1();
    void stopStreamFile();
    void addStreamDDC1(StreamDDC1*streamDDC1);
    void addFileStream(StreamFile *fileStream);
    std::shared_ptr<RingPacketBuffer>getDDC1Buffer();
private:
    void incomingConnection(qintptr handle);
private:
    StreamAnalizator * streamAnalizator;
    QList<StreamDDC1*>streamDDCList;
    QList<StreamFile*>fileStreamList;
    std::shared_ptr<CohG35DeviceSet>deviceSet;
};
#endif // STREAM_SERVE_RTEST_H
