#include "host_server.h"
#include "host_ds.h"

//************************** SERVER *************************
/*!
 * \addtogroup server
 */
///@{
HostServer::HostServer() {}

void HostServer::onChannelDisconnected()
{
    qDebug() << "onChannelDisconnected";
    DeviceClient* ds = qobject_cast<DeviceClient*>(sender());
    if(ds)
    {
        _client.removeOne(ds);
        ds->deleteLater();
    }
}

void HostServer::createSession(net::ChannelHost* channelHost)
{
    if(channelHost->sessionType() == SessionType::SESSION_COMMAND)
    {
        DeviceClient* deviceSetClient = new DeviceClient(channelHost);
        connect(deviceSetClient, &DeviceClient::deviceDisconnected,
                this, &HostServer::onChannelDisconnected);
        deviceSetClient->sendDeviceSetInfo();
        _client.append(deviceSetClient);
    }
    else
    {
        qDebug() << "ERROR SESSION TYPE";
    }
}
///@}
