#ifndef CLIENT_STREAM_CONTROLLER_H
#define CLIENT_STREAM_CONTROLLER_H

#include <QObject>

#include <memory>
class StreamReader;
class RingBuffer;
#include <QDebug>

class ClientStreamController:public QObject
{
    Q_OBJECT
public:
    ClientStreamController(const QString &deviceSetAddress,quint16 deviceSetClient);
    ~ClientStreamController();
    void startDDC();
    void stopDDC();
    inline std::shared_ptr<RingBuffer> ddc1Buffer(){
//        qDebug()<<"CSC:"<<_ddcBuffer.use_count();
        return _ddcBuffer;
    }
signals:
    void ddcStarted();
    void ddcStoped();
private:
    QString _address;
    quint16 _port;

    std::shared_ptr<RingBuffer>_ddcBuffer;
    StreamReader *_ddcStream;
    QThread *_ddcStreamThread;
};

#endif // CLIENT_STREAM_CONTROLLER_H
