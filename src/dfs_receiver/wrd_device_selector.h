#ifndef WRD_DEVICE_SELECTOR_H
#define WRD_DEVICE_SELECTOR_H

#include <memory>
#include "wrd_device.h"
#include "receiver.pb.h"
#include "ring_buffer.h"

using ShPtrDevice=std::shared_ptr<IDevice>;
using ShPtrRingBuffer=std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class DeviceCreator
{
public:
    ShPtrDevice create(unsigned int deviceIndex,const ShPtrRingBuffer &buffer){
       ShPtrDevice device=createDevice(deviceIndex,buffer);
       return device;
    }
    virtual  ShPtrDevice createDevice(unsigned int deviceIndex,
                                      const ShPtrRingBuffer &buffer)=0;
};

class CohG35DeviceCreator:public DeviceCreator
{
public:
    virtual  ShPtrDevice createDevice(unsigned int deviceIndex,
                                      const ShPtrRingBuffer &buffer)override;

};

class SingleG35DeviceCreator:public DeviceCreator
{
public:
    virtual  ShPtrDevice createDevice(unsigned int deviceIndex,
                                      const ShPtrRingBuffer &buffer)override;

};

class DeviceSelector
{
public:
    static ShPtrDevice singleG35DeviceInstance(unsigned int deviceIndex);

    static ShPtrDevice coherentG35DeviceInstance(unsigned int deviceSetIndex);

};
#endif // WRD_DEVICE_SELECTOR_H
