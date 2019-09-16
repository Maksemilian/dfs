#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include <QTcpSocket>

class PeerWireClient:public QTcpSocket
{
    Q_OBJECT

public:
    PeerWireClient(QObject *parent=nullptr);
    ~PeerWireClient();

    ///**************WRITE/READ****************
    qint64 readFromSocket(qint64 bytes);
    qint64 writeToSocket(qint64 bytes);

    qint64 readDataFromBuffer(char *data, qint64 size);
    qint64 writeDataToBuffer(const char *data, qint64 size);

    bool   canTransferMore();
    qint64 socketBytesAvailable();

private:
    QByteArray incomingBuffer;
    QByteArray outgoingBuffer;
};

#endif // CLIENT_SOCKET_H
