#include "sync_channel.h"

RadioChannel::RadioChannel(const ShPtrPacketBuffer& buffer, const SyncData& data)
    : _inBuffer(buffer), _data(data)
{

}

void RadioChannel::skip()
{
    if(!_queue.isEmpty())
    {
        _outBuffer->push( _queue.dequeue());
    }
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
