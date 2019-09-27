#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include <QListWidget>

class DeviceSetWidget;

class DeviceSetListWidget : public QListWidget
{
    Q_OBJECT
public:
    DeviceSetListWidget();
    void addDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
    void removeDeviceSetWidget(DeviceSetWidget*deviceSetWidget);
public slots:
    void connectToSelectedDeviceSet();
private slots:
    void onStationItemSelected(QListWidgetItem *item);

};

#endif // DEVICE_SET_WIDGET_LIST_H
