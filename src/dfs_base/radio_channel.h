#ifndef RADIO_CHANNEL_H
#define RADIO_CHANNEL_H

#include "ring_buffer.h"
#include "receiver.pb.h"
#include <QQueue>

using RingPacketBuffer = RingBuffer<proto::receiver::Packet>;
using ShPtrPacketBuffer = std::shared_ptr<RingPacketBuffer>;
using PacketQueue = QQueue<proto::receiver::Packet>;
using PacketQueuePair = std::pair<PacketQueue, PacketQueue>;

/*! \defgroup base Base
 * \image html class_base.jpg "Диаграмма классов модуля base"
 * \brief Модуль статической бибилотеки вспомогательных классов
 */

/*!
 * \brief Данные которые канал должен считывать
 */
struct ChannelData
{
    quint32 ddcFrequency = 5000000;
    quint32 sampleRate = 25000;
    quint32 blockSize = 8192;
};
///@{
/*!
 * \brief Класс радиоканала для чтения данных потока ddc1
 * \attention Данный класс может приводить к замеранию графического интерфейса
 * если вызывать его в главгном потоке
 */
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
    /*!
     * \brief читает данные из канала и поиещает считанные данные во
     * внутреннюю очередь
     * \return возвращает true если данные считаны успешно ,
     * в противном случае возвращает false
     */
    bool readIn();
    /*!
     * \brief перемещает данные из внутренней очереди в выходно буфер
     */
    void skip();

    /*!
     * \brief получает пакет из очереди
     * \param pct считанный пакет
     * \return возвращает true если данные считаны успешно ,
     * в противном случае возвращает false
     */
    inline bool getLastPacket(proto::receiver::Packet& pct)
    {
        if(!_queue.isEmpty())
        {
            pct = _queue.dequeue();
            return true;
        }
        return false;
    }
    /*!
     * \brief возвращает входной буфер канала куда приходят радиоданные
     * \return
     */
    const ShPtrPacketBuffer& inBuffer()
    {
        return _inBuffer;
    }
    /*!
     * \brief возвращае выходной буфер в который размещаются  счиатнные данные
     * из входного буфера
     * \return
     */
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
///@}
using ShPtrRadioChannel = std::shared_ptr<RadioChannel>;

#endif // RADIO_CHANNEL_H
