#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include "client_ds.h"
#include "receiver.pb.h"

#include <memory>

#include <QWidget>
#include <QObjectUserData>

class DeviceSetClient;

class QComboBox;
class QLabel;
class QLineEdit;

class DeviceSetWidget :public QWidget
{
    Q_OBJECT
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const QString TEXT_CONNECT;
    static const QString TEXT_DISCONNECT;
    static const int USER_DATA_STATUS=0;

    struct Status:public QObjectUserData
    {
        Status(bool status):_status(status){}
        ~Status() =default ;
        bool _status;
    };
public:
    DeviceSetWidget(const QString &address,quint16 port);
    void setAddress(const QString &address,quint16 port);
    QString address();
    quint16 port();
    void sendCommand( proto::receiver::Command &command);
    bool isConnected();
    void setListeningStreamPort(quint16 port);
    inline ShPtrPacketBuffer ddc1Buffer(){
        return _deviceSetClient->getDDC1Buffer();
    }
signals:
    void commandSuccessed();

public slots:
    void connectToDeviceSet();
    void disconnectFromDeviceSet();
private slots:
    void onDeviceSetReady();
    void onDeviceSetInfoUpdate();
    void onDeviceSetDisconnected();
    void onDeviceSetCommandFailed(const QString &errorString);
private:
    void setCursor(const QCursor &cursor);
    void setStatus(bool status);
private:
    QLabel *_lbAddresText;
    QLabel *_lbPort;
    QLabel *_lbStatus;
    QLabel *_lbStatusDDC1;
    QComboBox *_cbReceivers;
    QLineEdit *_leSetShiftPhaseDDC1;
    QComboBox *_cbDeviceSetIndex;
    std::unique_ptr<DeviceSetClient> _deviceSetClient;
};

#endif // DEVICE_SET_WIDGET_H
