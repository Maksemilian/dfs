#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "channel_host.h"

/*! \addtogroup net
 */
///@{
/*!
 * \brief Абстрактный базовый класс сервера
 */
class Server: public QTcpServer
{
    Q_OBJECT

  public:
    Server();
  signals:
    void newChannelReady();
  private:
    void incomingConnection(qintptr handle) override;
    void onChannelReady();
    void onNewConnection();
    virtual void createSession(net::ChannelHost* channelHost) = 0;
  private:
    QList<net::ChannelHost*>_pendingChannelsList;
    QList<net::ChannelHost*>_readyChannelsList;
};
///@}
#endif // SERVER_H
