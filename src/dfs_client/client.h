#ifndef CLIENT_H
#define CLIENT_H

#include "channel_client.h"

#include <receiver.pb.h>
#include <QObject>

class QHostAddress;

/*! \addtogroup client
 *
 */

///@{
/*!
 * \brief Данные сетевого соединения
 */
struct ConnectData
{
    QString address;
    quint16 port;
    SessionType type;
};

/*!
 * \brief Абстрактный базовый класс клиента
 */
class Client: public QObject
{
    Q_OBJECT
  public:
    Client(const ConnectData& connectData, QObject* parent = nullptr);
    /*!
     * \brief запускает работу клиента
     */
    void start();
    /*!
     * \brief останавливает работу клиента
     */
    void stop();
    /*!
     * \brief посылает сообщение по сети
     * \param message сообщение для отправки
     */
    void sendMessage(const google::protobuf::Message& message);
    virtual void onMessageReceived(const QByteArray& buffer) = 0;
    inline const ConnectData& connectData()
    {
        return _connectData;
    }
  signals:
    void started();
    void stoped();
  private:
    ConnectData _connectData;
    std::unique_ptr<net::ChannelClient> _channel;
};

///@}
#endif // CLIENT_H
