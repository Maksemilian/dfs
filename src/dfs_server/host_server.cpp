#include "host_server.h"
#include "host_ds.h"
#include "host_ds_stream.h"

#include <QThread>

//************************** SERVER *************************

StreamServer::StreamServer()
{
    qDebug()<<"Stream Server Init";
    connect(this,&StreamServer::newChannelReady,
            this,&StreamServer::onNewConnection);
}

void StreamServer::incomingConnection(qintptr handle)
{
    qDebug()<<"===============incomingConnection handle"<<handle;
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
    }else if(channelHost->sessionType()==SessionType::SESSION_SIGNAL_STREAM){
        qDebug()<<"STREAM SESSION";
        if(_streamDDC1==nullptr){
            createThread(channelHost);
        }else {
            //TODO СДЕЛАТЬ ОСТАНОВКУ ЧЕРЕЗ КОМАНДУ
            //ИЛИ ЧЕРЕЗ ПРОВЕРКУ СИГНАЛА
            _streamDDC1->stop();

            createThread(channelHost);
        }
    }else qDebug()<<"ERROR SESSION TYPE";
}

void StreamServer::createThread(net::ChannelHost *channelHost)
{
    QThread *thread=new QThread;
    _streamDDC1=new StreamDDC1(channelHost,_client->getCohDeviceSet()->getBuffer()/*deviceSet->getBuffer()*/);
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

StreamServer::~StreamServer()
{
}
