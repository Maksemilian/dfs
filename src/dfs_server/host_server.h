#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H

#include "server.h"

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
class HostServer: public Server
{
    Q_OBJECT

  public:
    HostServer();
  private:
    void createSession(net::ChannelHost* channelHost)override;
  private slots:
    void onChannelDisconnected();
  private:
    QList<DeviceClient*> _client;
};
///@}
#endif // STREAM_SERVE_RTEST_H
