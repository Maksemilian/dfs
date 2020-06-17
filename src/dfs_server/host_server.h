#ifndef STREAM_SERVE_RTEST_H
#define STREAM_SERVE_RTEST_H

#include "server.h"

class DeviceClient;

/*! \defgroup server Server
 * \image html class_server.jpg "Диаграмма классов модуля server"
 * \brief Консольное приложение обработки команд управления
 *  радиоприемными устройствами от удаленных клиентов
 */

///@{

/*!
 * \brief Класс для прослушивания входящих соединений
 * от удаленных клинтов.Как только соединение установлено ,
 * создается клиент DeviceClient , который отсылает информацию
 * об устройстве и ожидает приема команд.
 */
class HostServer: public net::Server
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
