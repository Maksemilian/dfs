#include "channel_client.h"

//#include "command.pb.h"

#include "key_exchange.pb.h"

#include "google/protobuf/message.h"

#include <QHostAddress>
#include <QDataStream>
#include <QDebug>

/*! \addtogroup net
 *
 */
namespace net
{

///@{
    namespace
    {

        const QByteArray serializeMessage(const google::protobuf::Message& message)
        {
            std::vector<char>bytesArray(static_cast<unsigned int>(message.ByteSize()));
            message.SerializeToArray(static_cast<void*>(bytesArray.data()), message.ByteSize());
            return QByteArray(bytesArray.data(), message.ByteSize());
        }

    }


    ChannelClient::ChannelClient(QObject* parent)
        : Channel (parent)
    {
        connect(_socket.get(), &QTcpSocket::connected,
                this, &ChannelClient::onConnected);
    }


    bool ChannelClient::isConnected()
    {
        if(_channelState == ChannelClient::ESTABLISHED)
        {
            return true;
        }

        return false;
    }

    void ChannelClient::connectToHost(const QString& address,
                                      quint16 port, SessionType sesionType)
    {
        qDebug() << "ChannelClient::connectToHost" << address
                 << "Port:" << port
                 << "SessionType:" << sesionType;
        _sessionType = sesionType;
        _socket->connectToHost(address, port);
    }

    void ChannelClient::disconnectFromHost()
    {
        _socket->disconnectFromHost();
        keyExchangeState = KeyExchangeState::HELLO;
        _channelState = ChannelState::NOT_CONNECTED;
    }

    void ChannelClient::waitForConnected(int time)
    {
        bool stat = _socket->waitForConnected(time);
        qDebug() << "StatWaitForConnected:" << stat;
    }

    void ChannelClient::waitForDisconnected(int time)
    {
        _socket->waitForDisconnected(time);
    }

    void ChannelClient::readServerKeyExchange(const QByteArray& buffer)
    {
        ServerKeyExchange serverKeyExchange;
        if(!serverKeyExchange.ParseFromArray(buffer.constData(), buffer.size()))
        {
            qDebug() << "ERROR PARSE SERVER_KEY_ECHANGE" << buffer.size();
            return;
        }

        qDebug() << "KEY_EXCHANGE : Host - > Client" <<
                 serverKeyExchange.method()
                 << serverKeyExchange.user_key();

        _userKey = serverKeyExchange.user_key();

        ClientKeyExchange clientKeyExchange;
        clientKeyExchange.set_method(serverKeyExchange.method());
        clientKeyExchange.set_user_key(_userKey);

        keyExchangeState = KeyExchangeState::KEY_EXCHANGE;

        writeToConnection(serializeMessage(clientKeyExchange));
    }

    void ChannelClient::readServerSessionChange(const QByteArray& buffer)
    {
        keyExchangeState = KeyExchangeState::SESSION;
        ServerSessionChange serverSessionChange;
        if(!serverSessionChange.ParseFromArray(buffer.constData(), buffer.size()))
        {
            qDebug() << "ERROR PARSE SERVER_SESSION_CHANGE" << buffer.size();
            return;
        }

        qDebug() << "SESSION_CHANGE : Host - > Client";

        ClientSessionChange clientSessionChange;
        clientSessionChange.set_session_type(_sessionType);
        keyExchangeState = KeyExchangeState::DONE;
        writeToConnection(serializeMessage(clientSessionChange));
        _channelState = ChannelState::ESTABLISHED;
        emit connected();
    }

    void ChannelClient::internalMessageReceive(const QByteArray& buffer)
    {
//    qDebug()<<"ChannelClient::internalMessageReceive"<<keyExchangeState<<buffer.size();
        switch (keyExchangeState)
        {
            case KeyExchangeState::HELLO:
                readServerKeyExchange(buffer);
                break;
            case KeyExchangeState::KEY_EXCHANGE:
                readServerSessionChange(buffer);
                break;

            default:
                break;
        }
    }

    void ChannelClient::onConnected()
    {
        _channelState = ChannelState::CONNECTED;
        qDebug() << "ChannnelClient::onConneted send HELLO";
        ClientHello clientHello;
        clientHello.set_method(Method::METHOD_ADDRESS);
        clientHello.set_user_name(_socket->peerAddress().toString().toStdString());
        writeToConnection(serializeMessage(clientHello));
    }

    ChannelClient::~ChannelClient()
    {
        qDebug() << "ChannelClient::destroyed";
    }
///@}

}
