#ifndef CHANNEL_CLIENT_H
#define CHANNEL_CLIENT_H

#include "channel.h"
#include "key_exchange.pb.h"

class ChannelClient:public Channel
{
    Q_OBJECT
public:
    ChannelClient(QObject *parent=nullptr);
    ChannelClient(qintptr handle,QObject *parent=nullptr);
    ~ChannelClient()override;
    void connectToHost(const QString &address,quint16 port,SessionType sesionType);
signals:
    void connected();
private:
    void internalMessageReceive(const QByteArray &buffer)override;
    void readServerKeyExchange(const QByteArray &buffer);
    void readServerSessionChange(const QByteArray &buffer);
private slots:
    void onConnected();
private:
    quint32 _userKey;
    SessionType _sessionType=SessionType::SESSION_UNKNOWN;
};

#endif // CHANNEL_CLIENT_H
