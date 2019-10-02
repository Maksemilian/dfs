#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include <QWidget>
#include <memory>
#include <QObjectUserData>

#include "client_device_set.h"
#include "receiver.pb.h"

class QLabel;
class QComboBox;
class DeviceSetClient;

class DeviceSetWidget :public QWidget
{
    Q_OBJECT
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const int USER_DATA_STATUS=0;

    struct Status:public QObjectUserData
    {
        Status(bool status):_status(status){}
        ~Status() =default ;
        bool _status;
    };
public:
    DeviceSetWidget(const QString &address,quint16 port);
    void setAddres(const QString &address,quint16 port);
    QString address();
    quint16 port();
    void setDeviceSetCommand(const proto::receiver::Command &command);
    bool isConnected();
signals:
    void commandSuccessed();

public slots:
    void connectToDeviceSet();
    void disconnectFromDeviceSet();
private slots:
    void onDeviceSetReady();
    void onDeviceSetDisconnected();
    void onDeviceSetCommandFailed(const QString &errorString);
private:
    void setCursor(const QCursor &cursor);
    void setStatus(bool status);
private:
    QLabel *_lbAddresText;
    QLabel *_lbPort;
    QLabel *_lbStatus;
    QComboBox *_cbReceivers;
    std::unique_ptr<DeviceSetClient>_deviceSetClient;
};

#endif // DEVICE_SET_WIDGET_H
