#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "channel_host.h"
#include "channel_client.h"

#include "wrd_coh_g35_ds.h"

#include <memory>

#include <QObject>
#include <QHostAddress>

using ShPtrRingBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;
/*!
 * \addtogroup server
 */

///@{

/*!
 * \brief Клиент для упраления приемными устройствами по сети
 * Принимает и выполняет команды над устройством ,
 * переданные по сети от удаленного клиента. Данный класс способен
 * отсылать данные потока DDC1 по запросу от подключившегося
 * клиента в реальном времени
 *
 * \attention допускается работа только с одним устройством.
 * По умолчанию данный класс работает только с перым
 * набором приемников WRG35DDC1 в когерентом режиме.
 *
 * \todo 1)выбор устройства по индексу \n
 *       2)переключение режима работы с когерентного на одиночный
 */
class DeviceClient: public QObject
{
    Q_OBJECT

    static const int SLEEP_TIME = 100;
  public:
    DeviceClient(net::ChannelHost* channelHost);

    ~DeviceClient();

    void sendDevieSetStatus();
    /*!
     * \brief отправляет информацию о наборе устройств
     */
    void sendDeviceSetInfo();
  signals:
    void deviceDisconnected();
    void changedDeviceSet(unsigned int indexDeviceSet);
  private slots:
    void onDisconnected();
    void onMessageReceived(const QByteArray& buffer);
  private:
    void setDeviceInfoCoherent();
    void sendCommandAnswer(proto::receiver::Answer* commandAnswer);
    void readCommandPacket(const proto::receiver::Command& command);
    DeviceSettings extractSettingsFromCommand(const proto::receiver::Command& command);

    void writeMessage(const google::protobuf::Message& message);
    void startSendingDDC1Stream(const QHostAddress& address, quint16 port,
                                const ShPtrRingBuffer& buffer);
  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};

/*!
 * \brief Класс для отправки данных потока.
 * Данные берутся из кольцевого буфера и отправляются по сети.
 */
class SignalStreamWriter: public QObject
{
    Q_OBJECT
  public:
    SignalStreamWriter(const QHostAddress& address, quint16 port,
                       const ShPtrRingBuffer& buffer);

    ~SignalStreamWriter()
    {
        qDebug() << "DESTR_DDC1";
    }

    /*!
     * \brief запускает поток отправки данных по сети
     * удаленному клиенту в отдельном потоке.
     */
    void start();
    /*!
     * \brief останавливает поток отправки данных.
     */
    void stop();
  signals:
    void finished();
  private:
    void process();
  private:
    std::unique_ptr<net::ChannelClient>_streamSocket;
    QHostAddress _address;
    quint16 _port;
    ShPtrRingBuffer _buffer;
    std::atomic<bool>_quit;
};
///@}
#endif // RECEIVER_STATION_CLIENT_H
