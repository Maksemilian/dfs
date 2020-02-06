#ifndef RADIO_CHANNEL_H
#define RADIO_CHANNEL_H

#include "ring_buffer.h"
#include "receiver.pb.h"
#include <QQueue>

using RingPacketBuffer = RingBuffer<proto::receiver::Packet>;
using ShPtrPacketBuffer = std::shared_ptr<RingPacketBuffer>;
using PacketQueue = QQueue<proto::receiver::Packet>;
using PacketQueuePair = std::pair<PacketQueue, PacketQueue>;

struct ChannelData
{
    quint32 ddcFrequency = 5000000;
    quint32 sampleRate = 25000;
    quint32 blockSize = 8192;
};

class RadioChannel
{
  public:
    RadioChannel(const ShPtrPacketBuffer& _inBuffer, const ChannelData& data);
    RadioChannel( const ChannelData& data = ChannelData(), quint32 bufferSize = 16);
    inline void setChannelData(const ChannelData& data)
    {
        _data = data;
    }
    inline ChannelData getChannelData()
    {
        return _data;
    }

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

    const ShPtrPacketBuffer& inBuffer()
    {
        return _inBuffer;
    }
    const ShPtrPacketBuffer& outBuffer()
    {
        return _outBuffer;
    }
  private:
    ShPtrPacketBuffer _inBuffer;
    ShPtrPacketBuffer _outBuffer;
    PacketQueue _queue;
    ChannelData _data;
};

using ShPtrRadioChannel = std::shared_ptr<RadioChannel>;

#endif // RADIO_CHANNEL_H
