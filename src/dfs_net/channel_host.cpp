#include "channel_host.h"
#include "command.pb.h"
#include "key_exchange.pb.h"

#include <QDebug>
#include <QByteArray>
#include <QHostAddress>

/*! \addtogroup net
 *
 */
///@{

namespace net
{

    namespace
    {

        const QByteArray serializeMessage(const google::protobuf::Message& message)
        {
            std::vector<char>bytesArray(static_cast<unsigned int>(message.ByteSize()));
            message.SerializeToArray(static_cast<void*>(bytesArray.data()), message.ByteSize());
            return QByteArray(bytesArray.data(), message.ByteSize());
        }

    }

    ChannelHost::ChannelHost(QObject* parent)
        : Channel (parent)
    {

    }

    ChannelHost::ChannelHost(qintptr handle, QObject* parent)
        : Channel (handle, parent)
    {

    }

    void ChannelHost::readClientHello(const QByteArray& buffer)
    {
        ClientHello clientHello;
        _channelState = Channel::CONNECTED;
        if(!clientHello.ParseFromArray(buffer.constData(), buffer.size()))
        {
            qDebug() << "ERROR PARSE CLIENT_HELLO" << clientHello.method();
            return;
        }
        qDebug() << "HELLO:Client - > Host" << clientHello.method();
        switch (clientHello.method())
        {
            case Method::METHOD_ADDRESS:
                _userKey = QHostAddress(clientHello.user_name().data()).toIPv4Address();
                break;
            case Method::METHOD_RANDOW_VALUE:
                break;
            default:
                break;
        }

        ServerKeyExchange serverKeyExchange;
        serverKeyExchange.set_method(clientHello.method());
        serverKeyExchange.set_user_key(_userKey);

        keyExchangeState = KeyExchangeState::KEY_EXCHANGE;

        writeToConnection(serializeMessage(serverKeyExchange));
    }

    void ChannelHost::readClientKeyExchange(const QByteArray& buffer)
    {
        ClientKeyExchange clientKeyExchange;
        if(!clientKeyExchange.ParseFromArray(buffer.constData(), buffer.size()))
        {
            qDebug() << "ERROR PARSE CLIENT_KEY_ECHANGE";
            return;
        }
        qDebug() << "KEY_ECHANGE:Client - > Host";

        if(clientKeyExchange.user_key() != _userKey)
        {
            qDebug() << "ERROR CLIENT_KEY AND SERVER NOT EQUAL";
            return;
        }

        //    emit keyExchangedFinished();
        keyExchangeState = KeyExchangeState::SESSION;
        ServerSessionChange serverSessionChange;
        serverSessionChange.set_session_type(SessionType::SESSIOM_TYPE_ALL);
        writeToConnection(serializeMessage(serverSessionChange));
    }

    void ChannelHost::readClientSession(const QByteArray& buffer)
    {
        ClientSessionChange clientSessionChange;

        if(!clientSessionChange.ParseFromArray(buffer.constData(), buffer.size()))
        {
            qDebug() << "ERROR PARSE CLIENT_SESSION_CHANGE";
            return;
        }

        if(clientSessionChange.session_type() == SessionType::SESSION_COMMAND ||
                clientSessionChange.session_type() == SessionType::SESSION_SIGNAL_STREAM ||
                clientSessionChange.session_type() == SessionType::SESSION_SIGNAL_FILE_TRANSFER
          )
        {
            _sessionType = clientSessionChange.session_type();
            keyExchangeState = KeyExchangeState::DONE;
            qDebug() << "SESSION DONE " << _userKey << _sessionType;
            _channelState = Channel::ESTABLISHED;
            emit keyExchangedFinished();
        }
        else
        {
            qDebug() << "ERROR SESSION TYPE";
        }
    }

    void ChannelHost::internalMessageReceive(const QByteArray& buffer)
    {
//        qDebug()<<"ChannelHost::internalMessageReceive"<<keyExchangeState;
        switch(keyExchangeState)
        {
            case KeyExchangeState::HELLO:
                readClientHello(buffer);
                break;
            case KeyExchangeState::KEY_EXCHANGE:
                readClientKeyExchange(buffer);
                break;
            case KeyExchangeState::SESSION:
                readClientSession(buffer);
                break;

            default:
                break;
        }
    }

}
///@}
