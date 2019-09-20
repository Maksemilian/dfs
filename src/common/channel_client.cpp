#include "channel_client.h"

#include "command.pb.h"
#include "key_exchange.pb.h"

#include "google/protobuf/message.h"

#include <QHostAddress>
#include <QDataStream>
#include <QDebug>

namespace  {

const QByteArray serializeMessage(const google::protobuf::Message &message)
{
    std::vector<char>bytesArray(static_cast<unsigned int>(message.ByteSize()));
    message.SerializeToArray(static_cast<void*>(bytesArray.data()),message.ByteSize());
    return QByteArray(bytesArray.data(),message.ByteSize());
}

}

ChannelClient::ChannelClient(QObject *parent)
    :Channel (parent),sessionType(SessionType::SESSION_COMMAND)
{
    connect(_socket.get(),&QTcpSocket::connected,this,&ChannelClient::onConnected);
}

ChannelClient::ChannelClient(qintptr handle,QObject *parent):Channel (handle,parent)
{

}

void ChannelClient::connectToHost(const QString &address, quint16 port)
{
    qDebug()<<"ChannelClient::connectToHost"<<address<<port;
    _socket->connectToHost(address,port);
}

void ChannelClient::readServerKeyExchange(const QByteArray &buffer)
{
    ServerKeyExchange serverKeyExchange;
    if(!serverKeyExchange.ParseFromArray(buffer.constData(),buffer.size())){
        qDebug()<<"ERROR PARSE SERVER_KEY_ECHANGE"<<buffer.size();
        return;
    }

//    qDebug()<<"KEY_EXCHANGE : Host - > Client"<<serverKeyExchange.method()
//           <<serverKeyExchange.user_key().data();

    user_key=serverKeyExchange.user_key();

    ClientKeyExchange clientKeyExchange;
    clientKeyExchange.set_method(serverKeyExchange.method());
    clientKeyExchange.set_user_key(user_key);

    keyExchangeState=KeyExchangeState::KEY_EXCHANGE;

    writeToConnection(serializeMessage(clientKeyExchange));
}

void ChannelClient::readServerSessionChange(const QByteArray &buffer)
{
    ServerSessionChange serverSessionChange;
    if(!serverSessionChange.ParseFromArray(buffer.constData(),buffer.size())){
         qDebug()<<"ERROR PARSE SERVER_SESSION_CHANGE"<<buffer.size();
         return;
    }

    ClientSessionChange clientSessionChange;
    clientSessionChange.set_session_type(sessionType);
    keyExchangeState=KeyExchangeState::SESSION;
    writeToConnection(serializeMessage(clientSessionChange));
}

void ChannelClient::internalMessageReceive(const QByteArray &buffer)
{
//    qDebug()<<"ChannelClient::internalMessageReceive"<<keyExchangeState<<buffer.size();
    switch (keyExchangeState) {
    case KeyExchangeState::HELLO:
        readServerKeyExchange(buffer);
        break;
    case KeyExchangeState::KEY_EXCHANGE:
        readServerSessionChange(buffer);
        break;

    default:break;
    }
}

void ChannelClient::onConnected()
{
    qDebug()<<"ChannnelClient::onConneted send HELLO";
    ClientHello clientHello;
    clientHello.set_method(Method::METHOD_ADDRESS);
    clientHello.set_user_name(_socket->peerAddress().toString().toStdString());
    writeToConnection(serializeMessage(clientHello));
}

ChannelClient::~ChannelClient()
{
    qDebug()<<"ChannelClient::destroyed";
}
