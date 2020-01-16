#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include "client_ds.h"

#include "receiver.pb.h"
#include "i_deviceset.h"

#include <memory>
#include <QListWidget>
#include <QQueue>

using namespace std;

class DeviceWidget;
class QPushButton;
class SwitchButton;
class IDeviceSettings;
class QListWidgetItem;
class DeviceListWidget :
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
    DeviceListWidget(QWidget* parent = nullptr);
    ~DeviceListWidget() override;
    void addDeviceSetWidget(const ConnectData& connectData, DeviceWidget* deviceSetWidget);
    void removeDeviceSetWidget(DeviceWidget* deviceSetWidget);
    void setCommand(proto::receiver::Command& command)override;
  signals:
    void commandSucessed()override;
    void allConnectedState(bool state);
    void ready(const std::vector<ShPtrPacketBuffer>& buffers);
    void notReady();
  public slots:
    void startDeviceClients();
    void stopDeviceClients();
  private slots:
    void onAddDevice();
    void onRemoveDevice();
  private:
    void setAllDeviceSet(proto::receiver::Command& command);

    void setCursor(const QCursor& cursor);

    void loadSettings();
    void saveSettings();
  private:
    QListWidget* _listWidget;
    SwitchButton* _pbConnectToStation;
    std::map<QListWidgetItem*, std::shared_ptr<DeviceSetClient> > _deviceses;

    QQueue<proto::receiver::Command>_commandQueue;
    int _counter = 0;
};

#endif // DEVICE_SET_WIDGET_LIST_H
