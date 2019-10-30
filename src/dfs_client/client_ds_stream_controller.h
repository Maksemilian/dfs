#ifndef CLIENT_STREAM_CONTROLLER_H
#define CLIENT_STREAM_CONTROLLER_H

#include "ring_buffer.h"
#include "receiver.pb.h"
#include <QObject>
#include <memory>
#include <QDebug>
class StreamReader;

class ClientStreamController:public QObject
{
    Q_OBJECT
public:
    ClientStreamController(const QString &deviceSetAddress,quint16 deviceSetClient);
    ~ClientStreamController();
    void startDDC();
    void stopDDC();
    inline std::shared_ptr<RingBuffer<proto::receiver::Packet>> ddc1Buffer(){
//        qDebug()<<"CSC:"<<_ddcBuffer.use_count();
        return _ddcBuffer;
    }
signals:
    void ddcStarted();
    void ddcStoped();
private:
    QString _address;
    quint16 _port;

    std::shared_ptr<RingBuffer<proto::receiver::Packet>>_ddcBuffer;
    StreamReader *_ddcStream;
    QThread *_ddcStreamThread;
};

#endif // CLIENT_STREAM_CONTROLLER_H
