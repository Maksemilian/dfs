#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include <QWidget>
#include <memory>
#include "core/network/device_set_client.h"
#include "receiver.pb.h"

class QLabel;
class QComboBox;
class DeviceSetClient;

class DeviceSetWidget :public QWidget
{
    Q_OBJECT
public:
    DeviceSetWidget(const QString &address,quint16 port);
    void setAddres(const QString &address,quint16 port);
    QString address();
    quint16 port();
    void setDeviceSetCommand(const proto::receiver::Command &command);
signals:
    void commandSuccessed();
public slots:
    void connectToDeviceSet();
private slots:
    void onDeviceSetReady();
    void onDeviceSetDisconnected();
    void onDeviceSetCommandFailed(const QString &errorString);

private:
    QLabel *_lbAddresText;
    QLabel *_lbPort;
    QComboBox *_cbReceivers;
    std::unique_ptr<DeviceSetClient>_deviceSetClient;
};

#endif // DEVICE_SET_WIDGET_H
