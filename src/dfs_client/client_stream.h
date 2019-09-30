#ifndef STREAM_CLIEINT_H
#define STREAM_CLIEINT_H

#include "i_stream_reader.h"
struct DeviceSetSettings;

class RingPacketBuffer;

class QHostAddress;

class StreamReader:IStreamRead
{
public:
    StreamReader(const QHostAddress &address,quint16 port,
                 const std::shared_ptr<RingPacketBuffer>&streamBuffer);
    bool readDDC1StreamData(Packet&data) override;
    void start();
    void stop();
    void resetBuffer();
private:
    void run();
private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // STREAM_CLIEINT_H
