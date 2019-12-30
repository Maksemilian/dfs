#ifndef WRD_CALLBACK_H
#define WRD_CALLBACK_H

#include "ring_buffer.h"
#include "receiver.pb.h"

class CohG35Callback;
class G35Callback;

using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class CallbackFactory
{
  public:
    static std::unique_ptr<CohG35Callback>
    cohG35CallbackInstance(const ShPtrRingPacketBuffer& ddc1Buffer);

    static std::unique_ptr<G35Callback>
    singleG35CallbackInstance(const ShPtrRingPacketBuffer& ddc1Buffer);
};
#endif // WRD_CALLBACK_H
