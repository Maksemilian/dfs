#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include "client_ds.h"
#include "receiver.pb.h"
#include <QQueue>
#include <QObject>

/*! \addtogroup client
*/
///@{

/*!
 * \brief Класс для управления клиентами устройств.
 * Позволяет добавлять и удалять клиентов для рассылки
 * им сообщений.
 */
class ClientManager: public QObject
{
    Q_OBJECT
  public:
    ClientManager();
    void startClients();
    void stopClients();
    void addClient(const ShPtrDeviceClient& client);
    void removeClient(const ShPtrDeviceClient& client);
    void setCommand(proto::receiver::Command& command);
    void clear();
    inline const std::set<ShPtrDeviceClient>& clients()
    {
        return _clients;
    }
    inline size_t countClients()
    {
        return _clients.size();
    }
  signals:
//    void ready(const std::vector<ShPtrRadioChannel>& buffers);
    void ready();
    void notReady();
  private:
    void checkConnection();
    void sendCommandForAll(proto::receiver::Command& command);
    void onCommandSuccessed();
  private:
    std::set<ShPtrDeviceClient> _clients;
    QQueue<proto::receiver::Command>_commandQueue;
    int _counter = 0;
};
///@}
using ShPtrClientManager = std::shared_ptr<ClientManager>;

#endif // CLIENT_MANAGER_H
