#include "echo_server.h"


#include "channel_host.h"

EchoServer::EchoServer()
{

}

void EchoServer::incomingConnection(qintptr handle)
{
    qDebug()<<"Echoserver::incomingConnection"<<handle;
    Channel *channel=new ChannelHost(handle);
}
