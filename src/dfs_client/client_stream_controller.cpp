#include "client_stream_controller.h"
#include "client_stream.h"
#include "ring_buffer.h"

#include <QThread>
#include <QDebug>

ClientStreamController::ClientStreamController(const QString &address,quint16 port)
    :_address(address),
      _port(port),
      _ddcBuffer(std::make_shared<RingBuffer>(16))
{

}

ClientStreamController::~ClientStreamController(){}
void ClientStreamController::startDDC()
{
//    if(_ddcStreamThread==nullptr&&_ddcStream==nullptr){

    _ddcStreamThread=new QThread;
    _ddcStream=new StreamReader(_address,_port,_ddcBuffer);

    connect(_ddcStream,&StreamReader::started,
            this,&ClientStreamController::ddcStarted);

    connect(_ddcStream,&StreamReader::stoped,
            this,&ClientStreamController::ddcStoped);

    connect(_ddcStreamThread,&QThread::started,
            _ddcStream,&StreamReader::start);

    connect(_ddcStream,&StreamReader::stoped,
            _ddcStreamThread,&QThread::quit);

    connect(_ddcStreamThread,&QThread::finished,
            _ddcStreamThread,&QThread::deleteLater);

    connect(_ddcStreamThread,&QThread::finished,
            _ddcStream,&StreamReader::deleteLater);

    _ddcStream->moveToThread(_ddcStreamThread);
    _ddcStreamThread->start();

      qDebug()<<"*********************STREAM CONTROLLER START DDC";
//    }
}

void ClientStreamController::stopDDC()
{
    _ddcStream->stop();
    _ddcBuffer->reset();
    qDebug()<<"*********************STREAM CONTROLLER STOPED DDC";
}
