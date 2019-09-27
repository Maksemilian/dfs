#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include <QListWidget>

#include "receiver.pb.h"
#include "interface/ideviceset.h"

class DeviceSetWidget;

class DeviceSetListWidget : public QListWidget,public IDeviceSet
{
    Q_OBJECT
public:
    DeviceSetListWidget();
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
    int counter=0;
};

#endif // DEVICE_SET_WIDGET_LIST_H
