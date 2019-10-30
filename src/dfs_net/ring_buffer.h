#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QWaitCondition>
#include <QReadLocker>
#include <QWriteLocker>

template<class T>
class RingBuffer
{
public:
    RingBuffer(int bufferSize):
        _array(new T[bufferSize]),
        _writeCounter(0),
        _readCounter(0),
        _mask(bufferSize-1),
        _size(bufferSize){  }

    void push(const T &packet){
        QWriteLocker writeLocker(&_mutex);
        _array[_writeCounter++ &_mask]=packet;
        _notEmpty.wakeOne();
    }

    bool pop(T&packet){
        QReadLocker readLocker(&_mutex);
        if( _readCounter<_writeCounter){
            packet=_array[_readCounter++ &_mask];
            return true;
        }
        else{
            _notEmpty.wait(&_mutex,2);
            return false;
        }
    }
    void reset(){
        QReadLocker readLocker(&_mutex);
        _writeCounter=0;
        _readCounter=0;
    }

    int getIndexRead(){
        return _readCounter;
    }
    int getIndexWrite(){
        return _writeCounter;
    }
    inline bool isReadable(){return _readCounter<_writeCounter;}
    int size(){return _size;}
private:
    T *_array;
    std::atomic<int> _writeCounter;
    std::atomic<int> _readCounter;
    std::atomic<int> _mask;
    int _size=0;
    QReadWriteLock _mutex;
    QWaitCondition _notEmpty;
};

//    bool get(int &index,Packet&packet){
//        QReadLocker readLocker(&mutex);
//        if(index<writeCounter){
//            //qDebug()<<"Comp"<<index<<writeCounter;
//            packet=array[index++ &mask];
//            return true;
//        }
//        else
//        {
//            notEmpty.wait(&mutex,2);
//            return false;
//        }
//    }
#endif // RINGBUFFER_H
