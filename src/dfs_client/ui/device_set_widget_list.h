#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H


#include "receiver.pb.h"
#include "interface/ideviceset.h"

#include <QListWidget>
#include <QQueue>
class DeviceSetWidget;

class DeviceSetListWidget : public QListWidget,public IDeviceSet
{
    Q_OBJECT
public:
    DeviceSetListWidget(QWidget*parent=nullptr);
    void addDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void removeDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void setCommand(const proto::receiver::Command &command)override;
signals:
    void commandSucessed()override;
public slots:
    void connectToSelectedDeviceSet();
private slots:
    void onStationItemSelected(QListWidgetItem *item);
private:
    void setAllDeviceSet(const proto::receiver::Command &command);
private:
    int _counter=0;
    QQueue<proto::receiver::Command>_commandQueue;
};

#endif // DEVICE_SET_WIDGET_LIST_H
