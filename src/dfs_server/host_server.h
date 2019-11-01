#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H


#include "host_ds.h"
#include "channel_host.h"
#include "host_ds_stream.h"

#include <memory>

#include <QTcpServer>
#include <QFutureWatcher>
#include <QQueue>
#include <QMutex>

class CohG35DeviceSet;
class ConnectRequest;
class StreamServer;

class StreamDDC1;
class ChannelHost;

class StreamServer:public QTcpServer
{
    Q_OBJECT

public:
    StreamServer(std::shared_ptr<CohG35DeviceSet>deviceSet);
    ~StreamServer()override;

    void addStreamDDC1(StreamDDC1*streamDDC1);
    void stopStreamDDC1();
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
    DeviceSetClient *_client;
    StreamDDC1 *_streamDDC1=nullptr;
    QList<StreamDDC1*>streamDDCList;
    std::shared_ptr<CohG35DeviceSet>deviceSet;
};

#endif // STREAM_SERVE_RTEST_H
