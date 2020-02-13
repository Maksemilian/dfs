#ifndef RECEIVER_STATION_CLIENT_H
#define RECEIVER_STATION_CLIENT_H

#include "radio_channel.h"

#include "client.h"

#include <QObject>

class QHostAddress;

/*! \addtogroup client
 */

///@{

/*!
 * \brief Класс реализует отпраук команд для управления
 * устройством
 */
class DeviceClient: public Client
{
    Q_OBJECT
    static quint8 _CLIENT_COUNTER;
    static const quint16 LISTENING_STREAMING_PORT = 9000;
  public:
    DeviceClient(const ConnectData& connectData, QObject* parent = nullptr);
    ~DeviceClient();

    void sendCommand(proto::receiver::Command& command);

    const proto::receiver::DeviceSetInfo& getDeviceSetInfo()const;
    QString getCurrentDeviceSetName()const;

    QStringList receiverNameList()const;
    ShPtrRadioChannel getDDC1Channel()const;
  signals:
    void commandSuccessed();
    void commandFailed(const QString& errorString);

    void deviceReady(const QStringList& receivers);
    void deviceInfoUpdated(const QStringList& receivers);

    void ddc1StreamStarted();
    void ddc1StreamStoped();

  private:
    void readAnswerPacket(const proto::receiver::Answer& answer);
    QString errorString(proto::receiver::CommandType commandType);
    void showCommandAnswer(proto::receiver::CommandType commandType);
  private slots:
    void onMessageReceived(const QByteArray& buffer);
  private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
///@}

using ShPtrDeviceClient = std::shared_ptr<DeviceClient>;
#endif // RECEIVER_STATION_CLIENT_H
