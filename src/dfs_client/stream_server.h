#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "server.h"
#include "radio_channel.h"

/*! \addtogroup client
 */

///@{

/*!  \brief Принимает входящие соединения для
   * от удаленного сервера для приема потока DDC1.
   */
class StreamServer : public net::Server
{
    Q_OBJECT
  public:
    enum class StreamType {ST_DDC1};
    std::vector<ShPtrRadioChannel> getRadioChannels();
  private:
    void createSession(net::ChannelHost* channelHost)override;
    void createThread(net::ChannelHost* channelHost);
  private:
    std::map<quint32, ShPtrRadioChannel> _radioChannels;
};

/*! \brief Принимает данные потока DDC1 по сети в реальном времени
* и записывает их в кольцевой буфер.
*/
class StreamReader: public QObject
{
    Q_OBJECT
  public:
    StreamReader(net::ChannelHost* channelHost,
                 const ShPtrPacketBuffer streamBuffer);
    ~StreamReader();
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
