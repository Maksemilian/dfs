#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "channel_host.h"

/*! \addtogroup net
 *
 */

namespace net
{
///@{
    /*!
     * \brief Абстрактный базовый класс сервера
     *
     * Ожидает новые подключения. Как только приходит новое подключение
     * оно добавляется список соединений ожидающих успешного подтверждения.
     * После подверждения соединение попадает в список соединений готовых к работе.
     * Каждый наследних класса Server должен переопределить
     * абстрактный метод createSession.
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
}
#endif // SERVER_H
