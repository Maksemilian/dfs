#include <QCoreApplication>

#include "channel_client.h"
#include "echo_server.h"
#include "key_exchange.pb.h"
#include "receiver.pb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    quint16 port=9090;
    EchoServer server;
    if(server.listen(QHostAddress("192.168.10.11"),port))
        qDebug()<<"success";


    ChannelClient *client=new ChannelClient;
    client->connectToHost("192.168.10.11",port,SessionType::SESSION_COMMAND);
    ServerKeyExchange serverKeyExchange;
    ClientKeyExchange clientKeyExchange;
    ServerSessionChange serverSessionChange;
    ClientSessionChange clientSessionChange;
    return a.exec();
}
