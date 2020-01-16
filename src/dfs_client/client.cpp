#include "client.h"

#include <QHostAddress>

const QByteArray serializeMessage(
    const google::protobuf::Message& command)
{
    std::vector<char>bytesArray(static_cast<unsigned int>(command.ByteSize()));
    command.SerializeToArray(static_cast<void*>(bytesArray.data()), command.ByteSize());
    return QByteArray(bytesArray.data(), command.ByteSize());
}

Client::Client(const ConnectData& connectData, QObject* parent):
    QObject (parent),
    _connectData(connectData),
    _channel(std::make_unique<net::ChannelClient>())
{
    setObjectName(connectData.address);
    connect(_channel.get(), &net::ChannelClient::messageReceived,
            this, &Client::onMessageReceived);

    connect(_channel.get(), &net::ChannelClient::connected,
            this, &Client::started);

    connect(_channel.get(), &net::ChannelClient::finished,
            this, &Client::stoped);
}

void Client::start()
{
    _channel->connectToHost(_connectData.address,
                            _connectData.port,
                            _connectData.type);

}

void Client::stop()
{
    _channel->disconnectFromHost();
}

void Client::sendMessage(const google::protobuf::Message& message)
{
    _channel->writeToConnection(serializeMessage(message));
}
