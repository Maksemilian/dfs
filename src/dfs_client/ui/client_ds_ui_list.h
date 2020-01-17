#ifndef DEVICE_SET_WIDGET_LIST_H
#define DEVICE_SET_WIDGET_LIST_H

#include "client_manager.h"

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
    public QWidget
{
    Q_OBJECT
    static const QString SETTINGS_FILE;
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const quint16 PORT = 9000;

    static const int TIME_WAIT_CONNECTION = 5000;
    static const int TIME_CHECK = 500;
  public:
    DeviceListWidget(const std::shared_ptr<ClientManager>& clientManager,
                     QWidget* parent = nullptr);
    ~DeviceListWidget() override;

    void addDeviceWidget(const ConnectData& connectData, DeviceWidget* deviceSetWidget);
    void removeDeviceWidget(DeviceWidget* deviceSetWidget);

  private slots:
    void onAddDevice();
    void onRemoveDevice();
  private:
    void onItemCliecked(QListWidgetItem* item);
    void loadSettings();
    void saveSettings();
  private:
    QListWidget* _listWidget;
    SwitchButton* _pbConnectToStation;
    std::map<QListWidgetItem*, std::shared_ptr<DeviceSetClient> > _deviceses;

    std::shared_ptr<ClientManager>_clientManager;
};

#endif // DEVICE_SET_WIDGET_LIST_H
