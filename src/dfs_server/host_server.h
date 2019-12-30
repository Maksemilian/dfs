#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H

#include "channel_host.h"

#include <QTcpServer>

class DeviceSetClient;

class StreamServer: public QTcpServer
{
    Q_OBJECT

  public:
    StreamServer();
  signals:
    void newChannelReady();
  private:
    void incomingConnection(qintptr handle) override;
    void onChannelReady();
    void onNewConnection();
    void onChannelDisconnected();
    void createSession(net::ChannelHost* channelHost);
  private:
    QList<net::ChannelHost*>_pendingChannelsList;
    QList<net::ChannelHost*>_readyChannelsList;
    QList<DeviceSetClient*> _client;
};

#endif // STREAM_SERVE_RTEST_H
