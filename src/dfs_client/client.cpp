#include "client.h"

#include <QHostAddress>

const QByteArray serializeMessage(
    const google::protobuf::Message& command)
{
    std::vector<char>bytesArray(static_cast<unsigned int>(command.ByteSize()));
    command.SerializeToArray(static_cast<void*>(bytesArray.data()), command.ByteSize());
    return QByteArray(bytesArray.data(), command.ByteSize());
}

Client::Client(QObject* parent):
    QObject (parent),
    channel(std::make_unique<net::ChannelClient>())
{
    connect(channel.get(), &net::ChannelClient::messageReceived,
            this, &Client::onMessageReceived);

    connect(channel.get(), &net::ChannelClient::connected,
            this, &Client::connected);

    connect(channel.get(), &net::ChannelClient::finished,
            this, &Client::disconnected);
}

void Client::connectToHost(const QHostAddress& address, quint16 port)
{
    setObjectName(address.toString());
    channel->connectToHost(address.toString(), port, SessionType::SESSION_COMMAND);
}

void Client::disconnectFromHost()
{
    channel->disconnectFromHost();
}

void Client::sendMessage(const google::protobuf::Message& message)
{
    channel->writeToConnection(serializeMessage(message));
}
