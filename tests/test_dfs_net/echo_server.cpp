#include "echo_server.h"


#include "channel_host.h"

EchoServer::EchoServer()
{

}

void EchoServer::incomingConnection(qintptr handle)
{
    qDebug()<<"Echoserver::incomingConnection"<<handle;
    net::Channel *channel=new net::ChannelHost(handle);
}
