#ifndef RING_PACKET_BUFFER_H
#define RING_PACKET_BUFFER_H

#include "packet.pb.h"

#include <QWaitCondition>
#include <QReadLocker>
#include <QWriteLocker>
//#include <QDebug>

class RingPacketBuffer{

public:
    RingPacketBuffer(int bufferSize):
        array(new Packet[bufferSize]),
        writeCounter(0),readCounter(0),mask(bufferSize-1){  }

    bool get(int &index,Packet&packet){
        QReadLocker readLocker(&mutex);
        if(index<writeCounter){
            //qDebug()<<"Comp"<<index<<writeCounter;
            packet=array[index++ &mask];
            return true;
        }
        else
        {
            notEmpty.wait(&mutex,2);
            return false;
        }
    }
    void push(const Packet &packet){
        QWriteLocker writeLocker(&mutex);
        array[writeCounter++ &mask]=packet;
//         qDebug("BUFFER_PUSH_WRITE");
        notEmpty.wakeOne();
    }

    bool pop(Packet&packet){
        QReadLocker readLocker(&mutex);
        if( readCounter<writeCounter){
            packet=array[readCounter++ &mask];
//            qDebug("BUFFER_POP_READ");
            return true;
        }
        else{
//            qDebug("BUFFER_POP_WAIT");
            notEmpty.wait(&mutex,2);
            return false;
        }
    }
    void reset(){
        QReadLocker readLocker(&mutex);
        writeCounter=0;
        readCounter=0;
    }

    int getIndexRead(){
        return readCounter;
    }
    int getIndexWrite(){
        return writeCounter;
    }
    inline bool isReadable(){return readCounter<writeCounter;}

private:
    Packet *array;
    std::atomic<int> writeCounter;
    std::atomic<int> readCounter;
    std::atomic<int> mask;

    QReadWriteLock mutex;
    QWaitCondition notEmpty;
};
#endif // RING_PACKET_BUFFER_H
