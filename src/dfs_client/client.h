#ifndef CLIENT_H
#define CLIENT_H

#include "channel_client.h"

#include <receiver.pb.h>
#include <QObject>

class QHostAddress;

class Client: public QObject
{
    Q_OBJECT
  public:
    Client(QObject* parent = nullptr);
    void connectToHost(const QString& address, quint16 port, SessionType type = SESSION_UNKNOWN);
    void disconnectFromHost();
    void sendMessage(const google::protobuf::Message& message);
    virtual void onMessageReceived(const QByteArray& buffer) = 0;
  signals:
    void connected();
    void disconnected();
  private:
    std::unique_ptr<net::ChannelClient> channel;
};

#endif // CLIENT_H
