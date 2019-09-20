#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include <QTcpServer>

class EchoServer : public QTcpServer
{
public:
    EchoServer();
private:
    void incomingConnection(qintptr handle)override;
};

#endif // ECHO_SERVER_H
