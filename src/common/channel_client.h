#ifndef CHANNEL_CLIENT_H
#define CHANNEL_CLIENT_H

#include "channel.h"
#include "key_exchange.pb.h"

class ChannelClient:public Channel
{
public:
    ChannelClient(QObject *parent=nullptr);
    ChannelClient(qintptr handle,QObject *parent=nullptr);
    ~ChannelClient()override;
    void connectToHost(const QString &address,quint16 port);
private:
    void internalMessageReceive(const QByteArray &buffer)override;
    void readServerKeyExchange(const QByteArray &buffer);
    void readServerSessionChange(const QByteArray &buffer);
private slots:
    void onConnected();
private:
    quint32 user_key;
    SessionType sessionType;
};

#endif // CHANNEL_CLIENT_H
