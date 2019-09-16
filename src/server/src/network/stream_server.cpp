#include "stream_server.h"

#include "ring_packet_buffer.h"
#include "src/receiver/coh_g35_device_set.h"

#include "peer_wire_client.h"
#include "stream_ddc1.h"
#include "stream_file.h"

#include "command.pb.h"

#include <QtConcurrent/QtConcurrent>
#include <QReadLocker>
#include <QMutexLocker>

//************************** SERVER *************************

StreamServer::StreamServer(std::shared_ptr<CohG35DeviceSet>deviceSet):
    deviceSet(deviceSet)
{
    streamAnalizator=new StreamAnalizator(this);
    streamAnalizator->start();
}

void StreamServer::incomingConnection(qintptr handle)
{
    qDebug()<<"===============incomingConnection handle"<<handle;
    streamAnalizator->pushSocketSescriptor(handle);
}

void StreamServer::addStreamDDC1(StreamDDC1 *stream)
{
    streamDDCList<<stream;
}

void StreamServer::addFileStream(StreamFile *fileStream)
{
    fileStreamList<<fileStream;
}

std::shared_ptr<RingPacketBuffer>StreamServer::getDDC1Buffer()
{
    return deviceSet->getDdc1Buffer();
}

void StreamServer::stopStreamDDC1()
{
    //WARNING ОТКЛЮЧАЕТСЯ ТОЛЬКО ПЕРВЫЙ
    streamDDCList.first()->stop();
}

void StreamServer::stopStreamFile()
{
    //WARNING ОТКЛЮЧАЕТСЯ ТОЛЬКО ПЕРВЫЙ
    fileStreamList.first()->setQuit(true);
}

StreamServer::~StreamServer()
{
    stopStreamDDC1();
    stopStreamFile();
    streamAnalizator->stop();
}

//************************ STREAM ANALIZATOR **************************

StreamAnalizator::StreamAnalizator(StreamServer*server):
    server(server)
{

}

void StreamAnalizator::start(){
    if(fw.isRunning())return;
    quit=false;
    fw.setFuture(QtConcurrent::run(this,&StreamAnalizator::run));
    qDebug("START STREAM WRITER");
}

void StreamAnalizator::stop(){
    quit=true;
    fw.waitForFinished();
    qDebug("STOP STREAM WRITER");
}

ConnectRequest StreamAnalizator::readStreamType(PeerWireClient *streamSocket)
{
    ConnectRequest request;
    char len[sizeof(int)];
    streamSocket->readFromSocket(sizeof(int));
    streamSocket->readDataFromBuffer(len,sizeof(int));
    bool ok;
    int num=QByteArray(len,sizeof(int)).toHex().toInt(&ok,16);
    std::vector<char> buf(static_cast<size_t>(num));
    streamSocket->readFromSocket(num);
    streamSocket->readDataFromBuffer(buf.data(),num);
    request.ParseFromArray(buf.data(),num);
    return  request;
}

void StreamAnalizator::sendAnswer(PeerWireClient *streamSocket,bool state)
{
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<state;

    streamSocket->writeDataToBuffer(baSize.constData(),baSize.size());
    streamSocket->writeToSocket(baSize.size());
    streamSocket->flush();
}

void StreamAnalizator::pushSocketSescriptor(qintptr socketDescriptor)
{
    QMutexLocker locker(&mutex);
    desckriptorQueue.enqueue(socketDescriptor);
}

bool StreamAnalizator::popSocketDescriptor(qintptr &socetDescriptor)
{
    QMutexLocker locker(&mutex);
    if(desckriptorQueue.size()>0){
        socetDescriptor=desckriptorQueue.dequeue();
        return true;
    }
    return false;
}

void StreamAnalizator::run(){
    qDebug()<<"***********StreamAnalizator::run";
    while(!quit){
        qintptr socketDescriptor=-1;
        if(popSocketDescriptor(socketDescriptor)){
            PeerWireClient *streamSocket=new PeerWireClient;
            streamSocket->setSocketDescriptor(socketDescriptor);
            qDebug()<<"****************StreamAnalizator::run-sd"<<socketDescriptor;
            if(streamSocket->waitForReadyRead(TIME_FOR_READY_READ_MS)){
                ConnectRequest request= readStreamType(streamSocket);
                if(request.type()==ConnectRequest::CT_STREAM){
                    qDebug("STREAM DDC MESSAGE");
                    sendAnswer(streamSocket,true);
                    createStreamDDC1(streamSocket);
                }else if (request.type()==ConnectRequest::CT_FILE) {
                    qDebug("STREAM FILE MESSAGE");
                    sendAnswer(streamSocket,true);
                    createFileStream(streamSocket);
                }
            }else{
                qDebug("ERROR STREAM");
                return;
            }
        }
    }
}

void StreamAnalizator::createStreamDDC1(PeerWireClient *streamSocket)
{
    QThread *thread=new QThread;
    StreamDDC1 *streamDDC1=new StreamDDC1(streamSocket,server->getDDC1Buffer());
    connect(thread,&QThread::started,streamDDC1,&StreamDDC1::process);
    connect(streamDDC1,&StreamDDC1::finished,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    streamSocket->moveToThread(thread);
    streamDDC1->moveToThread(thread);
    streamDDC1->start();

    server->addStreamDDC1(streamDDC1);
    thread->start();
}

void StreamAnalizator::createFileStream(PeerWireClient *streamSocket)
{
    QThread *thread=new QThread;
    StreamFile *fileStream=new StreamFile(streamSocket);

    connect(thread,&QThread::started,fileStream,&StreamFile::run);
    connect(fileStream,&StreamFile::finished,thread,&QThread::quit);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    streamSocket->moveToThread(thread);
    fileStream->moveToThread(thread);
    fileStream->setQuit(false);

    server->addFileStream(fileStream);
    thread->start();
}
