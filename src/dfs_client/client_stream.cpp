#include "client_stream.h"

#include "ring_packet_buffer.h"
#include "peer_wire_client.h"

#include "command.pb.h"

#include <QHostAddress>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>

struct StreamReader::Impl
{
    Impl(std::shared_ptr<RingPacketBuffer>ddcBuffer):
        streamBuffer(ddcBuffer)
    {}

    QHostAddress address;
    quint16 port;

    std::shared_ptr<RingPacketBuffer>streamBuffer;
    std::atomic<bool> quit;
    QFutureWatcher<void>streamFutureWatcher;
};

StreamReader::StreamReader(const QHostAddress &address, quint16 port,
                           const std::shared_ptr<RingPacketBuffer>&streamBuffer)
    :d(std::make_unique<Impl>(streamBuffer))
{
    d->address=address;
    d->port=port;
    //qDebug("CREATE STREAM DDC READER");
}

bool StreamReader::readDDC1StreamData(Packet &data)
{
    return d->streamBuffer->pop(data);
}

void StreamReader::start()
{
    if(d->streamFutureWatcher.isRunning())return;
    d->quit=false;
    d->streamFutureWatcher.setFuture( QtConcurrent::run(this,&StreamReader::run));
    qDebug("START STREAM READER");
}

void StreamReader::stop()
{
    d->quit=true;
    d->streamFutureWatcher.waitForFinished();
    qDebug("STOP STREAM READER");
}

void StreamReader::resetBuffer()
{
    qDebug()<<"StreamReader::resetBuffer";
    //TODO ПОДОБНЫЙ МЕТОД ЕСТЬ В SHARED_PTR КОТОРЫЙ
    //И ВЫЗЫВАЕТСЯ ИЗ ЗА ЧЕГО ПРОИСХОДИТ БАГ
    d->streamBuffer->reset();
}

void StreamReader::run()
{
    d->quit=false;
    PeerWireClient stream;
    stream.connectToHost(d->address,d->port);
    if(!stream.waitForConnected(1000)){
        qWarning()<<"ERROR connected for"<<d->address<<d->port;
        return;
    }else {
        qDebug()<<"SUCCESS connected for"<<d->address<<d->port;
    }
    //TODO СДЕЛАТЬ ПОТОК

    ConnectRequest request;
    request.set_type(ConnectRequest::CT_STREAM);

    int len=request.ByteSize();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<len;

    stream.writeDataToBuffer(baSize.constData(),baSize.size());
    stream.writeToSocket(baSize.size());

    char buf[request.ByteSize()];
    request.SerializeToArray(buf,request.ByteSize());
    stream.writeDataToBuffer(buf,request.ByteSize());
    stream.writeToSocket(request.ByteSize());
    stream.flush();
    //    qDebug()<<"StreamReader::write request";
    if(stream.waitForReadyRead(5000)){
        char buf[sizeof(int)];
        stream.readFromSocket(sizeof(int));
        stream.readDataFromBuffer(buf,sizeof(int));
        bool ok;
        int num=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        if(!num){
            //            qDebug()<<"*********NUM"<<num;
            return;
        }
        //        qDebug()<<"*****StreamReader::Read";
    }else{
        qDebug("Error Stream Reader");
        return;
    }

    qDebug()<<"THREAD_STREAM_READER********BEGIN DDC STREAM";
    qint64 bytesToRead=200000;
    int blockSize=0;
    int countBytesForRead=0;
    while(!d->quit){
        if(stream.waitForReadyRead(10)||stream.canTransferMore())
        {
            //qDebug()<<blockSize[i];
            if(blockSize==0){
                if(stream.socketBytesAvailable()>=sizeof(int)){
                    stream.readFromSocket(sizeof(int));
                    bool ok;
                    char buf[sizeof(int)];
                    stream.readDataFromBuffer(buf,sizeof(int));
                    blockSize=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
                    countBytesForRead=blockSize;
                    //qDebug()<<"Stream_FS"<<i<<"File Size"<<blockSize[i];
                }
            }
            else
            {
                qint64 available = qMin<qint64>(stream.socketBytesAvailable(), countBytesForRead);
                //qDebug()<<"Aval"<<available;
                if(available>0){
                    qint64 bytes=stream.readFromSocket(qMin<qint64>(available, bytesToRead));
                    if(bytes>0){
                        countBytesForRead-=bytes;
                    }
                }

                if(countBytesForRead==0){
                    char buf[blockSize];
                    stream.readDataFromBuffer(buf,blockSize);
                    Packet packet;
                    packet.ParseFromArray(buf,blockSize);
                    d->streamBuffer->push(packet);
                    qDebug()
                            <<"SD:"<<stream.socketDescriptor()
                           <<"BN:"<<packet.block_number()
                          <<"SR:"<<packet.sample_rate()
                         <<"TOW:"<<packet.time_of_week()
                        <<"DDC_C:"<<packet.ddc_sample_counter()
                       <<"ADC_C"<<packet.adc_period_counter();
                    blockSize=0;
                }
            }
        }
        //        else {
        //            qDebug()<<"ERROR:NO DATA";
        //        }
    }
    stream.disconnectFromHost();
    //stream.deleteLater();

    qDebug()<<"THREAD_STREAM_READER*******END DDC STREAM";
}
