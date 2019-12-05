#include "trmbl_tsip_reader.h"
#include "trmbl_tsip_parser.h"

#include <memory>

#include <QtConcurrent/QtConcurrent>
#include <QSerialPort>
#include <QSerialPortInfo>

using namespace std;

TimeReader &TimeReader::instance()
{
    static TimeReader timeReader;
    return timeReader;
}

TimeReader::TimeReader():startedAtomWatch(false),weekNumber(0),timeOfWeek(0){
}

void TimeReader::getTime(quint16 &weekNumber, quint32 &timeOfWeek)
{
    QReadLocker readLocker(&rwLock);
    weekNumber=this->weekNumber;
    timeOfWeek=this->timeOfWeek;
}

void TimeReader::getTimeOfWeek(quint32 &timeOfWeek){
    QReadLocker readLocker(&rwLock);
    timeOfWeek=this->timeOfWeek;
}

void TimeReader::getWeekNumber(quint16 &weekNumber){
    QReadLocker readLocker(&rwLock);
    weekNumber=this->weekNumber;
}

void TimeReader::start(){
    reset();
    startedAtomWatch=true;
    std::shared_ptr<QMetaObject::Connection> sharedPtrConnection(new QMetaObject::Connection) ;
    *sharedPtrConnection  =connect(&processFutureWatcher,&QFutureWatcher<void>::finished,[this,sharedPtrConnection]{
        disconnect(*sharedPtrConnection);
        emit stopedTrimble();
    });

    processFutureWatcher.setFuture(QtConcurrent::run(this,&TimeReader::process));
}

void TimeReader::stop(){
    startedAtomWatch=false;
    processFutureWatcher.waitForFinished();
}

void TimeReader::process(){
    QSerialPort comPort;

   for (QSerialPortInfo info:QSerialPortInfo::availablePorts())
   {
       if(!info.isBusy()){
           comPort.setPortName(info.portName());
           break;
       }
   }
    comPort.setBaudRate(QSerialPort::Baud9600);
    comPort.setDataBits(QSerialPort::Data8);
    comPort.setParity(QSerialPort::NoParity);
    comPort.setStopBits(QSerialPort::OneStop);
    comPort.setFlowControl(QSerialPort::NoFlowControl);
    if(comPort.open(QIODevice::ReadWrite))
    {
        qDebug("START SERIAL PORT");
        TsipParser parser;
        TsipParser::PacketAB packetAB;
        while(startedAtomWatch)
        {
            parser.receivePacket(&comPort);

            rwLock.lockForWrite();
            if (parser.getCurrentPacket()==TsipParser::PN_AB)
            {
                packetAB=parser.getPacketAB();
                timeOfWeek=packetAB.ulTimeOfWeek;
                weekNumber=packetAB.usWeekNumber;
            }
            rwLock.unlock();
        }
        qDebug("SIGNAL FINISHED SERIAL PORT");
    }
    else
    {
        qDebug()<<"SERIAL PORT ERROR"<<comPort.error()<<comPort.portName();
    }
    comPort.close();
}
