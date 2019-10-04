#ifndef STREAM_CLIEINT_H
#define STREAM_CLIEINT_H

//#include "i_stream_reader.h"
#include <QObject>
#include <memory>

class RingBuffer;
class QHostAddress;

class StreamReader:public QObject
{
    Q_OBJECT
public:
    StreamReader(const QString&address,quint16 port,
                 const std::shared_ptr<RingBuffer>streamBuffer);
    ~StreamReader();
    void start();
    void stop();
    void resetBuffer();
signals:
    void started();
    void stoped();
private:
    void run();
    void readStream();
    void onMessageReceive(const QByteArray&buffer);
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // STREAM_CLIEINT_H
