#ifndef CHANNEL_H
#define CHANNEL_H

#include <QTcpSocket>
#include <memory>

class Channel:public QObject
{
    Q_OBJECT
public:
    enum  KeyExchangeState {HELLO=0,KEY_EXCHANGE=1,SESSION=2,DONE=3};
    Channel(QObject*parent=nullptr);
    Channel(qintptr handle,QObject *parent=nullptr);
    ~Channel();
    virtual void internalMessageReceive(const QByteArray &buffer)=0;
    ///**************WRITE/READ****************
    void writeToConnection(const QByteArray &commandData);

    qint64 readFromSocket(qint64 bytes);
    qint64 writeToSocket(qint64 bytes);

    qint64 readDataFromBuffer(char *data, qint64 size);
    qint64 writeDataToBuffer(const char *data, qint64 size);

    bool   canTransferMore();
    qint64 socketBytesAvailable();
private:
    void onReadyRead();
    void onMessageReceive();
protected:
    std::unique_ptr<QTcpSocket> _socket;
    KeyExchangeState keyExchangeState=KeyExchangeState::HELLO;
private:
    QByteArray incomingBuffer;
    QByteArray outgoingBuffer;
    qint64 answerSize=0;
};


#endif // CHANNEL_H
