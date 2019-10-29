#ifndef INTERFACE_READABLE_STREAM_H
#define INTERFACE_READABLE_STREAM_H

#include <QObject>
#include <memory>

class Packet;

class IStreamRead
{
public:
    virtual ~IStreamRead()=default;

    virtual  bool readDDC1StreamData(Packet &data)=0;
};

using ShPtrBuffer=std::shared_ptr<IStreamRead>;
using ShPtrBufferPair=QPair<ShPtrBuffer,ShPtrBuffer>;

#endif // INTERFACE_READABLE_STREAM_H

