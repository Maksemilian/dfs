#ifndef WRD_DEVICE_SELECTOR_H
#define WRD_DEVICE_SELECTOR_H

#include <memory>

#include "wrd_device_interface.h"
#include "receiver.pb.h"
#include "ring_buffer.h"

using ShPtrDevice=std::shared_ptr<IDevice>;
using ShPtrRingBuffer=std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class DeviceFactory
{
public:
    static ShPtrDevice createCohG35Device(unsigned int deviceIndex,
                                          const ShPtrRingBuffer &buffer,
                                          bool demo_mode=false);

    static ShPtrDevice createSingleG35Device(unsigned int deviceIndex,
                                             const ShPtrRingBuffer &buffer,
                                             bool demo_mode=false);
};

#endif // WRD_DEVICE_SELECTOR_H
