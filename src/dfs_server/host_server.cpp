#include "host_server.h"

#include "device_set_coh_g35.h"

#include "host_ds_stream.h"
#include "channel_host.h"

#include "command.pb.h"

#include <QtConcurrent/QtConcurrent>
#include <QReadLocker>
#include <QMutexLocker>
#include <QThread>

//************************** SERVER *************************

StreamServer::StreamServer(std::shared_ptr<CohG35DeviceSet>deviceSet):
    deviceSet(deviceSet)
{
    qDebug()<<"Stream Server Init";
    //    streamAnalizator=new StreamAnalizator(this);
    //    streamAnalizator->start();
    connect(this,&StreamServer::newChannelReady,
            this,&StreamServer::onNewConnection);

}

void StreamServer::incomingConnection(qintptr handle)
{
    qDebug()<<"===============incomingConnection handle"<<handle;
    //    streamAnalizator->pushSocketSescriptor(handle);
    net::ChannelHost *channelHost=new net::ChannelHost(handle);
    connect(channelHost,&net::ChannelHost::keyExchangedFinished,this,&StreamServer::onChannelReady);
    connect(channelHost,&net::ChannelHost::finished,
            this,&StreamServer::onChannelDisconnected);
    _pendingChannelsList.append(channelHost);
}

void StreamServer::onChannelReady()
{
    qDebug()<<"Server::onChannelReady";

    auto it=_pendingChannelsList.begin();
    while (it!=_pendingChannelsList.end()) {
        net::ChannelHost* networkChannel=*it;

        if(!networkChannel)
        {
            it =_pendingChannelsList.erase(it);
        }
        else if(networkChannel->state()==net::ChannelHost::ESTABLISHED)
        {
            it =_pendingChannelsList.erase(it);
            //            qDebug()<<networkChannel<<*it;
            _readyChannelsList.append(networkChannel);
            emit newChannelReady();
        }
        else
        {
            it++;
        }
    }
}

void StreamServer::onNewConnection()
{
    qDebug()<<"Server::onNewConnection";

    while (!_readyChannelsList.isEmpty()) {
        net::ChannelHost* networkChannel=_readyChannelsList.front();
        qDebug()<<networkChannel->sessionType();
        if(networkChannel){
            qDebug()<<"Create Session";
            createSession(networkChannel);
            _readyChannelsList.pop_front();
        }else {
            qDebug()<<"NULLPTR SESSION TYPE";
        }
    }
}

void StreamServer::onChannelDisconnected()
{
    qDebug()<<"onChannelDisconnected";
    _client->deleteLater();
}

void StreamServer::createSession(net::ChannelHost *channelHost)
{
    if(channelHost->sessionType()==SessionType::SESSION_COMMAND){
        _client=new DeviceSetClient(channelHost);
        _client->sendDeviceSetInfo();
    }else if(channelHost->sessionType()==SessionType::SESSION_SIGNAL_STREAM){
        qDebug()<<"STREAM SESSION";
        if(_streamDDC1==nullptr){
            createThread(channelHost);
        }else {
            //TODO ИСПРАВИТЬ
            _streamDDC1->stop();

            createThread(channelHost);
        }
    }else qDebug()<<"ERROR SESSION TYPE";
}

void StreamServer::createThread(net::ChannelHost *channelHost)
{
    QThread *thread=new QThread;
    _streamDDC1=new StreamDDC1(channelHost,_client->getBuffer());
    _streamDDC1->moveToThread(thread);
    channelHost->moveToThread(thread);
//        connect(channelHost,&net::ChannelHost::finished,
//                [this]{
//           _streamDDC1->stop();
//        });
    connect(thread,&QThread::started,
            _streamDDC1,&StreamDDC1::start);

    connect(_streamDDC1,&StreamDDC1::finished,
            thread,&QThread::quit);

    connect(thread,&QThread::finished,
            _streamDDC1,&StreamDDC1::deleteLater);

    connect(thread,&QThread::destroyed,
            thread,&QThread::deleteLater);

    thread->start();
}

//void StreamServer::addStreamDDC1(StreamDDC1 *stream)
//{
//    streamDDCList<<stream;
//}

//void StreamServer::addFileStream(StreamFile *fileStream)
//{
//    fileStreamList<<fileStream;
//}

//std::shared_ptr<RingPacketBuffer>StreamServer::getDDC1Buffer()
//{
//    return deviceSet->getDdc1Buffer();
//}

void StreamServer::stopStreamDDC1()
{
    //WARNING ОТКЛЮЧАЕТСЯ ТОЛЬКО ПЕРВЫЙ
    streamDDCList.first()->stop();
}

//void StreamServer::stopStreamFile()
//{
//    //WARNING ОТКЛЮЧАЕТСЯ ТОЛЬКО ПЕРВЫЙ
//    fileStreamList.first()->setQuit(true);
//}

StreamServer::~StreamServer()
{
    stopStreamDDC1();
//    stopStreamFile();
//    streamAnalizator->stop();
}

/*
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
    //    QThread *thread=new QThread;
    //    StreamDDC1 *streamDDC1=new StreamDDC1(streamSocket,server->getDDC1Buffer());
    //    connect(thread,&QThread::started,streamDDC1,&StreamDDC1::process);
    //    connect(streamDDC1,&StreamDDC1::finished,thread,&QThread::quit);
    //    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

    //    streamSocket->moveToThread(thread);
    //    streamDDC1->moveToThread(thread);
    //    streamDDC1->start();

    //    server->addStreamDDC1(streamDDC1);
    //    thread->start();
}

void StreamAnalizator::createFileStream(PeerWireClient *streamSocket)
{
//    QThread *thread=new QThread;
//    StreamFile *fileStream=new StreamFile(streamSocket);

//    connect(thread,&QThread::started,fileStream,&StreamFile::run);
//    connect(fileStream,&StreamFile::finished,thread,&QThread::quit);
//    connect(thread,&QThread::finished,thread,&QThread::deleteLater);

//    streamSocket->moveToThread(thread);
//    fileStream->moveToThread(thread);
//    fileStream->setQuit(false);

//    server->addFileStream(fileStream);
//    thread->start();
}
*/
