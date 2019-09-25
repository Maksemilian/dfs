#ifndef CHANNEL_HOST_H
#define CHANNEL_HOST_H

#include "channel.h"
#include "key_exchange.pb.h"

class ChannelHost:public Channel
{
    Q_OBJECT
public:
    ChannelHost(QObject *parent=nullptr);
    ChannelHost(qintptr handle,QObject *parent=nullptr);
signals:
    void keyExchangedFinished();
private:
    void internalMessageReceive(const QByteArray &buffer)override;
    void readClientHello(const QByteArray &buffer);
    void readClientKeyExchange(const QByteArray &buffer);
    void readClientSession(const QByteArray &buffer);

private:
    quint32 user_key;
    SessionType sessionType=SessionType::SESSION_UNKNOWN;
};

#endif // CHANNEL_HOST_H
