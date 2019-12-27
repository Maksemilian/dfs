#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include "receiver.pb.h"
#include "i_deviceset.h"
#include "sync_pair_channel.h"

#include <QListWidget>
#include <QQueue>

class DeviceSetWidget;
class QPushButton;
class SwitchButton;
class IDeviceSettings;

class DeviceSetListWidget :
//        public QListWidget,
        public QWidget,
        public IDeviceSet
{
    Q_OBJECT
    static const QString SETTINGS_FILE;
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const quint16 PORT=9000;

    static const int TIME_WAIT_CONNECTION=5000;
    static const int TIME_CHECK=500;
public:
    DeviceSetListWidget(QWidget*parent=nullptr);
    ~DeviceSetListWidget();
    void addDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void removeDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void setCommand(proto::receiver::Command &command)override;
signals:
    void commandSucessed()override;
    void allConnectedState(bool state);
    void ready(const QList<DeviceSetWidget*>&dsList);
    void notReady();
public slots:
    void connectToSelectedDeviceSet();
    void disconnectFromSelectedDeviceSet();
private slots:
    void onStationItemSelected(QListWidgetItem *item);
    void onAddDeviceSetWidget();
    void onRemoveDeviceSetWidget();
    void onTest(bool state);
private:
    void setCursor(const QCursor &cursor);
    void setAllDeviceSet(proto::receiver::Command &command);
    void loadSettings();
    void saveSettings();
    void checkingConnections(bool checkingState);
    QList<DeviceSetWidget*> createdDeviceSetWidgets();
private:
    int _counter=0;
    QQueue<proto::receiver::Command>_commandQueue;
    QListWidget *_listWidget;
    SwitchButton *_pbConnectToStation;
    QList<DeviceSetWidget*>_allDeviceSetWidgetList;
    QList<DeviceSetWidget*>_connectedDeviceSetWidgetList;
};

#endif // DEVICE_SET_WIDGET_LIST_H
