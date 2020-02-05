#ifndef RADIO_CHANNEL_H
#define RADIO_CHANNEL_H

#include "sync_global.h"

class RadioChannel
{
  public:
    RadioChannel(const ShPtrPacketBuffer& _inBuffer, const SyncData& data);
    bool readIn();

    void skip();

    inline bool getLastPacket(proto::receiver::Packet& pct)
    {
        if(!_queue.isEmpty())
        {
            pct = _queue.dequeue();
            return true;
        }
        return false;
    }

    const ShPtrPacketBuffer& outBuffer()
    {
        return _outBuffer;
    }
  private:
    ShPtrPacketBuffer _inBuffer;
    ShPtrPacketBuffer _outBuffer;
    PacketQueue _queue;
    SyncData _data;
};
using ShPtrRadioChannel = std::shared_ptr<RadioChannel>;
#endif // RADIO_CHANNEL_H
