#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H

#include "channel_host.h"

#include <QTcpServer>

class DeviceClient;
/*! \defgroup server Server
 * \brief Консольное приложение обработки команд управления
 *  радиоприемными устройствами от удаленных клиентов
 */
///@{
/*!
 * \brief Класс для прослушивания входящих соединений
 * для удаленного упраления приемными устройствами
 *
 * Как только соединение установлено создается клиент DeviceClient
 * который принимает и выполняет команды по управлению приемным устройством
 * от удаленных клиентов
 */
class StreamServer: public QTcpServer
{
    Q_OBJECT

  public:
    StreamServer();
  signals:
    void newChannelReady();
  private:
    void incomingConnection(qintptr handle) override;
    void onChannelReady();
    void onNewConnection();
    void onChannelDisconnected();
    void createSession(net::ChannelHost* channelHost);
  private:
    QList<net::ChannelHost*>_pendingChannelsList;
    QList<net::ChannelHost*>_readyChannelsList;
    QList<DeviceClient*> _client;
};
///@}
#endif // STREAM_SERVE_RTEST_H
