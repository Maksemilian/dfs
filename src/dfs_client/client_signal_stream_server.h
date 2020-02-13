#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "server.h"

#include "radio_channel.h"

#include <QList>

/*! \addtogroup client
 */
///@{

//TODO Должен быть базовый класс сервера
// с чисто вертуальным методом createSession
/*!
 * \brief Класс принимает входящие соединения для
 * передающие поток ddc1
 */
class SignalStreamServer: public Server
{
    Q_OBJECT
  public:
    enum class StreamType {ST_DDC1};
    SignalStreamServer(quint8 bufferSize = 16);
    ShPtrRadioChannel getChannel(StreamType type);
  private:
    void createSession(net::ChannelHost* channelHost)override;
    void createThread(net::ChannelHost* channelHost);
  private:
    std::map<StreamType, ShPtrRadioChannel> buffers;
};

/*!
 * \brief Класс чтения потока ddc1 из сети
 */
class SignalStreamReader: public QObject
{
    Q_OBJECT
  public:
    SignalStreamReader(net::ChannelHost* channelHost,
                       const ShPtrPacketBuffer streamBuffer);
    ~SignalStreamReader();
    void process();
  signals:
    void finished();
  private:
    void onMessageReceive(const QByteArray& buffer);
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
///@}
#endif // STREAM_SERVER_H
