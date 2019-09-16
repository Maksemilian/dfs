#include "peer_wire_client.h"

#include <QDataStream>

PeerWireClient::PeerWireClient(QObject *parent):
    QTcpSocket(parent){}

///**************WRITE/READ****************

qint64 PeerWireClient::writeToSocket(qint64 bytes)
{
    qint64 totalWritten = 0;
    do {
        qint64 written = write(outgoingBuffer.constData(),
                                      qMin<qint64>(bytes - totalWritten, outgoingBuffer.size()));
        if (written <= 0)
            return totalWritten ? totalWritten : written;

        totalWritten += written;
        outgoingBuffer.remove(0, static_cast<int>(written));
    } while (totalWritten < bytes && !outgoingBuffer.isEmpty());

    return totalWritten;
}

qint64 PeerWireClient::readFromSocket(qint64 bytes){
    char buffer[1000];
    qint64 totalRead = 0;
    do {
        qint64 bytesRead = read(buffer, qMin<qint64>(sizeof(buffer), bytes - totalRead));
        if (bytesRead <= 0)
            break;
        qint64 oldSize = incomingBuffer.size();
        incomingBuffer.resize(static_cast<int>(oldSize + bytesRead));
        memcpy(incomingBuffer.data() + oldSize, buffer,static_cast<size_t>(bytesRead));

        totalRead += bytesRead;
    } while (totalRead < bytes);

    return totalRead;
}

qint64 PeerWireClient::writeDataToBuffer(const char *data, qint64 size)
{
    int oldSize = outgoingBuffer.size();
    outgoingBuffer.resize(static_cast<int>(oldSize + size));
    memcpy(outgoingBuffer.data() + oldSize, data,static_cast<size_t>(size));
    return size;
}

qint64 PeerWireClient::readDataFromBuffer(char *data, qint64 size)
{
    int n = qMin<int>(static_cast<int>(size), incomingBuffer.size());
    memcpy(data, incomingBuffer.constData(), static_cast<size_t>(n));
    incomingBuffer.remove(0, n);
    return n;
}

bool PeerWireClient::canTransferMore(){
    return bytesAvailable()>0;
}

qint64 PeerWireClient::socketBytesAvailable(){
    return bytesAvailable();
}

PeerWireClient::~PeerWireClient(){
    deleteLater();
}
