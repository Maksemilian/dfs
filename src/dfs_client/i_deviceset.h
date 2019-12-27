#ifndef IDEVICESET_H
#define IDEVICESET_H

#include "receiver.pb.h"

class IDeviceSet
{
public:
    virtual void setCommand(proto::receiver::Command& command)=0;
    virtual void commandSucessed()=0;

};
#endif // IDEVICESET_H
