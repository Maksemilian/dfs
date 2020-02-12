#include "host_server.h"
#include "host_ds.h"

//************************** SERVER *************************
/*!
 * \addtogroup server
 */
///@{
StreamServer::StreamServer()
{
    qDebug() << "Stream Server Init";
    connect(this, &StreamServer::newChannelReady,
            this, &StreamServer::onNewConnection);
}

void StreamServer::incomingConnection(qintptr handle)
{
    qDebug() << "===============incomingConnection handle" << handle;
    net::ChannelHost* channelHost = new net::ChannelHost(handle);

    connect(channelHost, &net::ChannelHost::keyExchangedFinished,
            this, &StreamServer::onChannelReady);

    _pendingChannelsList.append(channelHost);
}

void StreamServer::onChannelReady()
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

void StreamServer::onNewConnection()
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

void StreamServer::onChannelDisconnected()
{
    qDebug() << "onChannelDisconnected";
    DeviceClient* ds = qobject_cast<DeviceClient*>(sender());
    if(ds)
    {
        _client.removeOne(ds);
        ds->deleteLater();
    }
}

void StreamServer::createSession(net::ChannelHost* channelHost)
{
    if(channelHost->sessionType() == SessionType::SESSION_COMMAND)
    {
        DeviceClient* deviceSetClient = new DeviceClient(channelHost);
        connect(deviceSetClient, &DeviceClient::deviceDisconnected,
                this, &StreamServer::onChannelDisconnected);
        //TODO ОТПРАВИТЬ СООБЩЕНИЕ О ГОТОВНОСТИ
//        deviceSetClient->sendDevieSetStatus();
        deviceSetClient->sendDeviceSetInfo();
        _client.append(deviceSetClient);
    }
    else
    {
        qDebug() << "ERROR SESSION TYPE";
    }
}
///@}
