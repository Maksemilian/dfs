#include "sync_radio_channel.h"

RadioChannel::RadioChannel(const ShPtrPacketBuffer& buffer, const SyncData& data)
    : _inBuffer(buffer), _data(data)
{

}

bool RadioChannel::read()
{
    if(_inBuffer->pop(_lastPacket))
    {
        if(_lastPacket.sample_rate() == _data.sampleRate)
        {
            _queue.enqueue(_lastPacket);
            return true;
        }
    }
    return false;
}

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
