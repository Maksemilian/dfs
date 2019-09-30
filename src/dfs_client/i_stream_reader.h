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

using ShPtrStreamReadable=std::shared_ptr<IStreamRead>;
using StreamReadablePair=QPair<ShPtrStreamReadable,ShPtrStreamReadable>;

#endif // INTERFACE_READABLE_STREAM_H

