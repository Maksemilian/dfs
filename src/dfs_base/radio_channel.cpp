#include "radio_channel.h"

/*!
 * \addtogroup base
 */
///@{

RadioChannel::RadioChannel(const ShPtrPacketBuffer& buffer,
                           const ChannelData& data)
    : _inBuffer(buffer),
      _data(data)
{

}

RadioChannel::RadioChannel(const ChannelData& data, quint32 bufferSize)
    : _inBuffer(std::make_shared<RingBuffer<proto::receiver::Packet>>(bufferSize)),
      _outBuffer(std::make_shared<RingBuffer<proto::receiver::Packet>>(bufferSize)),

      _data(data)
{

}

void RadioChannel::skip()
{
    proto::receiver::Packet pct;
    if(getLastPacket(pct))
    {
        _outBuffer->push( pct);
    }
    /*
    if(!_queue.isEmpty())
    {
        _outBuffer->push( _queue.dequeue());
    }*/
}

bool RadioChannel::readIn()
{
    proto::receiver::Packet lastPacket;
    if(_inBuffer->pop(lastPacket))
    {
        if(lastPacket.sample_rate() == _data.sampleRate)
        {
            _queue.enqueue(lastPacket);
            return true;
        }
    }
    return false;
}
///@}
/*
void RadioChannel::apply()
{
    if(!_queue.isEmpty())
    {
        _lastPacket = _queue.dequeue();

        if(_blockEqualizer)
        {
            _blockEqualizer->equateT(_lastPacket);
        }
        _outBuffer->push(_lastPacket);
    }
}
*/
