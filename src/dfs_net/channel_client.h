#ifndef CHANNEL_CLIENT_H
#define CHANNEL_CLIENT_H

#include "channel.h"
#include "key_exchange.pb.h"

/*! \addtogroup net
 *
 */
///@{
namespace net
{
    /*!
     * \brief Реализует клиентское соединение к хосту
     *
     * Ведет сетевой обмен сообщениями с ChannelHost
     * до тех пор пока не будет установлено соединение
     */
    class ChannelClient: public Channel
    {
        Q_OBJECT
      public:
        ChannelClient(QObject* parent = nullptr);
        //ChannelClient(qintptr handle,QObject *parent=nullptr);
        ~ChannelClient()override;
        /*!
         * \brief подключение к удалленому хосту
         * \param address IP аддрес хоста
         * \param port порт хоста
         * \param sesionType тип соединения
         */
        void connectToHost(const QString& address, quint16 port, SessionType sesionType);
        void disconnectFromHost();
        void waitForConnected(int time = 30000);
        void waitForDisconnected(int time = 30000);
        bool isConnected();
      signals:
        void connected();
        void disconnected();
      private:
        void internalMessageReceive(const QByteArray& buffer)override;
        void readServerKeyExchange(const QByteArray& buffer);
        void readServerSessionChange(const QByteArray& buffer);
      private slots:
        void onConnected();
      private:
        quint32 _userKey;
        SessionType _sessionType = SessionType::SESSION_UNKNOWN;
    };
}
///@}
#endif // CHANNEL_CLIENT_H
