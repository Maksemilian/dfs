#include "channel.h"

#include <QDataStream>
#include <QHostAddress>

Channel::Channel(QObject *parent)
    : QObject(parent),_socket(std::make_unique<QTcpSocket>())
{
    connect(_socket.get(),&QTcpSocket::readyRead,this,&Channel::onReadyRead);
    connect(_socket.get(),&QTcpSocket::disconnected,this,&Channel::finished);
}

Channel::Channel(qintptr _handle,QObject *parent)
    :Channel(parent)
{
    _socket->setSocketDescriptor(_handle);
}

QHostAddress Channel::peerAddress()
{
    return _socket->peerAddress();
}

void Channel::writeToConnection(const QByteArray &commandData)
{
    //    qDebug()<<"Channel::writeToConnection";
    int byteSize=commandData.size();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<byteSize;

    qint64 bytesSize=writeDataToBuffer(baSize.constData(),baSize.size());
    writeToSocket(bytesSize);

    bytesSize=writeDataToBuffer(commandData.constData(),commandData.size());
    writeToSocket(bytesSize);

    _socket->flush();
}
///**************WRITE/READ****************

qint64 Channel::writeToSocket(qint64 bytes)
{
    qint64 totalWritten = 0;
    do {
        qint64 written =_socket->write(outgoingBuffer.constData(),
                                       qMin<qint64>(bytes - totalWritten, outgoingBuffer.size()));
        if (written <= 0)
            return totalWritten ? totalWritten : written;

        totalWritten += written;
        outgoingBuffer.remove(0, static_cast<int>(written));
    } while (totalWritten < bytes && !outgoingBuffer.isEmpty());

    return totalWritten;
}

qint64 Channel::readFromSocket(qint64 bytes)
{
    char buffer[1000];
    qint64 totalRead = 0;
    do {
        qint64 bytesRead = _socket->read(buffer, qMin<qint64>(sizeof(buffer), bytes - totalRead));
        if (bytesRead <= 0)
            break;
        qint64 oldSize = incomingBuffer.size();
        incomingBuffer.resize(static_cast<int>(oldSize + bytesRead));
        memcpy(incomingBuffer.data() + oldSize, buffer,static_cast<size_t>(bytesRead));

        totalRead += bytesRead;
    } while (totalRead < bytes);

    return totalRead;
}

qint64 Channel::writeDataToBuffer(const char *data, qint64 size)
{
    int oldSize = outgoingBuffer.size();
    outgoingBuffer.resize(static_cast<int>(oldSize + size));
    memcpy(outgoingBuffer.data() + oldSize, data,static_cast<size_t>(size));
    return size;
}

qint64 Channel::readDataFromBuffer(char *data, qint64 size)
{
    int n = qMin<int>(static_cast<int>(size), incomingBuffer.size());
    memcpy(data, incomingBuffer.constData(), static_cast<size_t>(n));
    incomingBuffer.remove(0, n);
    return n;
}

bool Channel::canTransferMore()
{
    return _socket->bytesAvailable()>0;
}

qint64 Channel::socketBytesAvailable()
{
    return _socket->bytesAvailable();
}

void Channel::onMessageReceive()
{
    //    qDebug()<<"Channel::answerSize"<<answerSize;
    QByteArray buffer;
    buffer.resize(static_cast<int>(answerSize));
    readDataFromBuffer(buffer.data(),buffer.size());

    if(keyExchangeState==KeyExchangeState::DONE){
        emit messageReceived(buffer);
    }else {
        internalMessageReceive(buffer);
    }
    //TODO СДЕЛАТЬ ОСВОБОЖДЕНИЕ БУФЕРА
    //    internalMessageReceive(incomingBuffer);
}

void Channel::onReadyRead()
{
    //    qDebug()<<"Channel::onReadyRead"<<answerSize;
    //static qint64 answerSize=0;// WARNING НЕ РАБОТАЕТ ДЛЯ КОЛИЧЕСТВА СТАНЦИЙ > 1
    if(answerSize==0&&
            socketBytesAvailable()>=sizeof(int)){
        readFromSocket(sizeof(int));
        char buf[sizeof(int)];
        readDataFromBuffer(buf,sizeof(int));
        bool ok;
        answerSize= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        //qDebug()<<"Size"<<size;
    }else if(socketBytesAvailable()>0){
        qint64 bytes=readFromSocket(answerSize);
        if(bytes==answerSize){
            //qDebug("Rec");
            onMessageReceive();
            answerSize=0;
            return;
        }
    }

    onReadyRead();
}

Channel::~Channel()
{
    deleteLater();
}


