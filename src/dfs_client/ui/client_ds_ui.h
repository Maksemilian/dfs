#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include "client_ds.h"
#include "receiver.pb.h"

#include <QWidget>
#include <QObjectUserData>
#include <memory>
#include "client_ds_stream_controller.h"
class DeviceSetClient;
class ClientStreamController;

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
    void setAddres(const QString &address,quint16 port);
    QString address();
    quint16 port();
    void sendCommand(const proto::receiver::Command &command);
    bool isConnected();
    inline std::shared_ptr<RingBuffer<proto::receiver::Packet>> ddc1Buffer(){
//        qDebug()<<"DSW:"<<_streamController->ddc1Buffer().use_count();
        return _streamController->ddc1Buffer();
    }
//    quint32 getDdc1Frequiency();
//    quint32 getSampleRate();
//    quint32 getBufferSize();
signals:
    void commandSuccessed();
public slots:
    void connectToDeviceSet();
    void disconnectFromDeviceSet();
private slots:
    void onDeviceSetReady();
    void onDeviceSetDisconnected();
    void onDeviceSetCommandFailed(const QString &errorString);
    void onDDC1Started();
    void onDDC1Stoped();
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
    std::unique_ptr<DeviceSetClient> _deviceSetClient;
    std::unique_ptr<ClientStreamController> _streamController;
};

#endif // DEVICE_SET_WIDGET_H
