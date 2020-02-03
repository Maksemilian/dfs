#ifndef SYNC_GLOBAL_H
#define SYNC_GLOBAL_H

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "ippbase.h"

#include <memory>

using RingIpp32fcBuffer = RingBuffer<std::vector<Ipp32fc>>;
using RingPacketBuffer = RingBuffer<proto::receiver::Packet>;

using ShPtrIpp32fcBuffer = std::shared_ptr<RingIpp32fcBuffer>;
using ShPtrPacketBuffer = std::shared_ptr<RingPacketBuffer>;

using ShPtrPacketBufferPair = std::pair<ShPtrPacketBuffer, ShPtrPacketBuffer>;
using VectorIpp32fc = std::vector<Ipp32fc>;
using BoolPair = std::pair<bool, bool>;

using PacketQueue = QQueue<proto::receiver::Packet>;
using PacketQueuePair = std::pair<PacketQueue, PacketQueue>;


using VectorIpp32fc = std::vector<Ipp32fc>;
using VectorIpp32f = std::vector<Ipp32f>;
#endif // SYNC_GLOBAL_H