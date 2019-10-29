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
class IDeviceSetSettings;

class DeviceSetListWidget :
//        public QListWidget,
        public QWidget,
        public IDeviceSet
{
    Q_OBJECT
    static const QString SETTINGS_FILE;
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const int TIME_WAIT_CONNECTION=5000;
    static const int TIME_CHECK=500;
public:
    DeviceSetListWidget(QWidget*parent=nullptr);
    void addDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void removeDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void setCommand(const proto::receiver::Command &command)override;
    //TODO PUBLIC MEMBER
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
    void setAllDeviceSet(const proto::receiver::Command &command);
    void createDevieSetWidgets();
    void checkingConnections(bool checkingState);
private:
    int _counter=0;
    QQueue<proto::receiver::Command>_commandQueue;
    QListWidget *_listWidget;
    SwitchButton *_pbConnectToStation;
    QList<DeviceSetWidget*>_deviceSetWidgetList;
};

#endif // DEVICE_SET_WIDGET_LIST_H
