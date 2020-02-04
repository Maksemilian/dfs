#ifndef RADIO_CHANNEL_H
#define RADIO_CHANNEL_H

#include "sync_global.h"
#include "sync_block_equalizer.h"

class RadioChannel
{
  public:
    RadioChannel(const ShPtrPacketBuffer& _inBuffer, const SyncData& data);
    bool read();
    void apply();

    inline const proto::receiver::Packet& lastPacket()
    {
        return _lastPacket;
    }

    inline void setBlockEqulizer(BlockEqualizer* be)
    {
        _blockEqualizer = be;
    }
    const ShPtrPacketBuffer& outBuffer()
    {
        return _outBuffer;
    }
  private:
    ShPtrPacketBuffer _inBuffer;
    ShPtrPacketBuffer _outBuffer;
    PacketQueue _queue;
    proto::receiver::Packet _lastPacket;
    SyncData _data;
    BlockEqualizer* _blockEqualizer;
};

#endif // RADIO_CHANNEL_H
