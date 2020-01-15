#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include "client_ds.h"

#include "receiver.pb.h"
#include "i_deviceset.h"

#include <memory>
#include <QListWidget>
#include <QQueue>

using namespace std;

class DeviceSetWidget;
class QPushButton;
class SwitchButton;
class IDeviceSettings;
class QListWidgetItem;
class DeviceSetListWidget :
//        public QListWidget,
    public QWidget,
    public IDeviceSet
{
    Q_OBJECT
    static const QString SETTINGS_FILE;
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const quint16 PORT = 9000;

    static const int TIME_WAIT_CONNECTION = 5000;
    static const int TIME_CHECK = 500;
  public:
    DeviceSetListWidget(QWidget* parent = nullptr);
    ~DeviceSetListWidget();
    void addDeviceSetWidget(DeviceSetWidget* deviceSetWidget);
    void removeDeviceSetWidget(DeviceSetWidget* deviceSetWidget);
    void setCommand(proto::receiver::Command& command)override;
  signals:
    void commandSucessed()override;
    void allConnectedState(bool state);
    void ready(const QList<DeviceSetWidget*>& dsList);
    void readyTest(const std::vector<ShPtrPacketBuffer>& buffers);
    void notReady();
  public slots:
    void connectToSelectedDeviceSet();
    void disconnectFromSelectedDeviceSet();
  private slots:
    void onDeviceSelected(QListWidgetItem* item);
    void onAddDevice();
    void onRemoveDevice();
    void onTest(bool state);

    void onDeviceSetUpdate(quint32 numberDeviceSet);
  private:
    void setAllDeviceSet(proto::receiver::Command& command);

    void setCursor(const QCursor& cursor);

    void loadSettings();
    void saveSettings();
  private:
    int _counter = 0;
    QQueue<proto::receiver::Command>_commandQueue;
    QListWidget* _listWidget;
    SwitchButton* _pbConnectToStation;
    QList<DeviceSetWidget*>_connectedDeviceSetWidgetList;

    std::map<QListWidgetItem*, std::shared_ptr<DeviceSetClient> > _deviceses;
    std::vector<std::shared_ptr<DeviceSetClient>>_connectedDevices;
};

#endif // DEVICE_SET_WIDGET_LIST_H
