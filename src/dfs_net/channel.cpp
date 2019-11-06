#include "channel.h"

#include <QDataStream>
#include <QHostAddress>
namespace net {

Channel::Channel(QObject *parent)
    : QObject(parent),
      _socket(std::make_unique<QTcpSocket>())
{
    connect(_socket.get(),&QTcpSocket::readyRead,
            this,&Channel::onReadyRead);

    connect(_socket.get(),&QTcpSocket::disconnected,
            //            this,&Channel::finished);
            [this]{
        keyExchangeState=KeyExchangeState::HELLO;
        _channelState=NOT_CONNECTED;
        emit finished();
    });
}

Channel::Channel(qintptr _handle,QObject *parent)
    :Channel(parent)
{
    _socket->setSocketDescriptor(_handle);
}

bool Channel::waitChannel(int msec)
{

    return _socket->waitForConnected(msec)?true:false;
}

bool Channel::isOpen()
{
    return  _socket->isOpen();
}

bool Channel::isWritable()
{
    return  _socket->isWritable();
}

bool Channel::flush()
{
    return    _socket->flush();
}

void Channel::moveToThread(QThread *thread)
{
    _socket->moveToThread(thread);
    this->QObject::moveToThread(thread);
}

QHostAddress Channel::peerAddress()
{
    return _socket->peerAddress();
}

void Channel::writeToConnection(const QByteArray &commandData)
{
    qDebug()<<"Channel::writeToConnection"<<commandData.size();
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
        qint64 oldSize = read_.buffer.size();
        read_.buffer.resize(static_cast<int>(oldSize + bytesRead));
        memcpy(read_.buffer.data() + oldSize, buffer,static_cast<size_t>(bytesRead));

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
    int n = qMin<int>(static_cast<int>(size), read_.buffer.size());
    memcpy(data, read_.buffer.constData(), static_cast<size_t>(n));
    read_.buffer.remove(0, n);
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
    buffer.resize(static_cast<int>(read_.buffer.size()));
    readDataFromBuffer(buffer.data(),buffer.size());

    if(keyExchangeState==KeyExchangeState::DONE){
//        qDebug()<<"WORK_MES"<<buffer.size();
        emit messageReceived(buffer);
    }else {
        qDebug()<<"SESSION_MES"<<buffer.size();
        internalMessageReceive(buffer);
    }
    //TODO СДЕЛАТЬ ОСВОБОЖДЕНИЕ БУФЕРА
    //    internalMessageReceive(incomingBuffer);
}

void Channel::onReadyRead()
{
    quint64 current = 0;
//    qDebug()<<"onReadyRead";
    for (;;) {
        if(!read_.buffer_size_received){

            quint8 byte;
            current = _socket->read(reinterpret_cast<char*>(&byte),
                                    sizeof(byte));
//            qDebug()<<"Current:"<<current;
            if (current == sizeof(byte))
            {
//                qDebug()<<read_.bytes_transferred<<current<<byte;
                read_.buffer+=byte;
                if ( read_.bytes_transferred == 3)
                {
                    bool ok;
                    read_.buffer_size=read_.buffer.toHex().toInt(&ok,16);
                    read_.buffer_size_received=true;
                    read_.buffer.clear();
//                    qDebug()<<"Size"<<read_.buffer_size<<current;
                    if (!read_.buffer_size || read_.buffer_size > 16 * 1024 * 1024)
                    {
                        //                        emit errorOccurred(Error::UNKNOWN);
                        qDebug()<<"ERROR CONNECT"<<read_.buffer_size;
                        return;
                    }


                    if(read_.buffer.capacity()<read_.buffer_size)
                        read_.buffer.reserve(read_.buffer_size);

                    read_.buffer.resize(read_.buffer_size);
                    read_.buffer_size=0;
                    read_.bytes_transferred=0;

                    continue;
                }
            }
        }
        else if(read_.bytes_transferred<read_.buffer.size()){
            current=_socket->read(read_.buffer.data()+read_.bytes_transferred,
                                  read_.buffer.size()-read_.bytes_transferred);
//            qDebug()<<"R:"<<current<<read_.bytes_transferred;
        }
        else {
            read_.buffer_size_received=false;
            read_.bytes_transferred=0;

            onMessageReceive();
            return;
        }

        if (current <= 0)
            return;

        read_.bytes_transferred += current;
    }
}

Channel::~Channel()
{
    deleteLater();
}

}

//     qDebug()<<"Channel::onReadyRead"<<answerSize;
//    //static qint64 answerSize=0;// WARNING НЕ РАБОТАЕТ ДЛЯ КОЛИЧЕСТВА СТАНЦИЙ > 1
//    if(answerSize==0&&
//            socketBytesAvailable()>=sizeof(int)){
//        readFromSocket(sizeof(int));
//        char buf[sizeof(int)];
//        readDataFromBuffer(buf,sizeof(int));
//        bool ok;
//        answerSize= QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
//        qDebug()<<"Size"<<answerSize;
//    }else if(socketBytesAvailable()>0){
//        qint64 bytes=readFromSocket(answerSize);
//        if(bytes==answerSize){
//            //qDebug("Rec");
//            onMessageReceive();
//            answerSize=0;
//            return;
//        }else {
//            qDebug()<<"BA"<<bytes;
//        }
//    }else {
//            qDebug()<<"SOCK_BA"<<socketBytesAvailable();
//    }

//    onReadyRead();
