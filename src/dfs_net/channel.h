#ifndef CHANNEL_H
#define CHANNEL_H

#include <QTcpSocket>
#include <memory>

class Channel:public QObject
{
    Q_OBJECT
public:
    enum  KeyExchangeState {HELLO=0,KEY_EXCHANGE=1,SESSION=2,DONE=3};
    enum  ChannelState { NOT_CONNECTED,CONNECTED,ESTABLISHED};
    Channel(QObject*parent=nullptr);
    Channel(qintptr handle,QObject *parent=nullptr);
    void writeToConnection(const QByteArray &commandData);
    inline ChannelState state(){return  _channelState ;}
    QHostAddress peerAddress();
    ~Channel();
    bool waitChannel(int msec);
signals:
    void messageReceived(const QByteArray &buffer);
    void finished();
protected:
    virtual void internalMessageReceive(const QByteArray &buffer)=0;
    ///**************WRITE/READ****************

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
    ChannelState _channelState=ChannelState::NOT_CONNECTED;
private:
    QByteArray incomingBuffer;
    QByteArray outgoingBuffer;
    qint64 answerSize=0;
};


#endif // CHANNEL_H
