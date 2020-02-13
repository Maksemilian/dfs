#include "server.h"
#include "channel_host.h"

Server::Server()
{
    qDebug() << "Stream Server Init";
    connect(this, &Server::newChannelReady,
            this, &Server::onNewConnection);
}

void Server::incomingConnection(qintptr handle)
{
    qDebug() << "===============incomingConnection handle" << handle;
    net::ChannelHost* channelHost = new net::ChannelHost(handle);

    connect(channelHost, &net::ChannelHost::keyExchangedFinished,
            this, &Server::onChannelReady);

    _pendingChannelsList.append(channelHost);
}

void Server::onChannelReady()
{
    qDebug() << "Server::onChannelReady";

    auto it = _pendingChannelsList.begin();
    while (it != _pendingChannelsList.end())
    {
        net::ChannelHost* networkChannel = *it;

        if(!networkChannel)
        {
            it = _pendingChannelsList.erase(it);
        }
        else if(networkChannel->state() == net::ChannelHost::ESTABLISHED)
        {
            it = _pendingChannelsList.erase(it);
            //            qDebug()<<networkChannel<<*it;
            _readyChannelsList.append(networkChannel);
            emit newChannelReady();
        }
        else
        {
            it++;
        }
    }
}

void Server::onNewConnection()
{
    qDebug() << "Server::onNewConnection";

    while (!_readyChannelsList.isEmpty())
    {
        net::ChannelHost* networkChannel = _readyChannelsList.front();
        qDebug() << "Session Type" << networkChannel->sessionType();
        if(networkChannel && networkChannel->sessionType() ==
                SessionType::SESSION_COMMAND)
        {
            qDebug() << "Create Session";
            createSession(networkChannel);
            _readyChannelsList.pop_front();
        }
        else
        {
            qDebug() << "BAD SESSION TYPE:" << networkChannel->sessionType();
        }
    }
}

//void Server::onChannelDisconnected()
//{
//    qDebug() << "onChannelDisconnected";
//    DeviceClient* ds = qobject_cast<DeviceClient*>(sender());
//    if(ds)
//    {
//        _client.removeOne(ds);
//        ds->deleteLater();
//    }
//}

