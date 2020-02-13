#ifndef TREE_DEVICES_H
#define TREE_DEVICES_H

#include "client_manager.h"

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QSplitter>
/*! \addtogroup client
 */
///@{
class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;
class DeviceListWidget;
class DeviceWidget;

class DeviceMonitoring :
//public QStackedWidget
    public QSplitter
{
    static const QString SETTINGS_TREE_FILE_NAME;
    static const int MAX_COUNT_SELECTED_DEVICE_SET = 1;
    static const QString SETTINGS_LIST_FILE_NAME;

  public:
    DeviceMonitoring(const ShPtrClientManager& clientManager, QWidget* parent = nullptr);
    ~DeviceMonitoring()override;
  protected:
    bool eventFilter(QObject* obj, QEvent* event)override;
  private:
//    void addDeviceWidget(const ConnectData& connectData,
//                         DeviceWidget* deviceSetWidget);

    void createDeivceWidgets();
//    void startClients();
//    void stopClients();

//    void loadListSettings();
//    void saveListSettings();

    void saveTreeSettings();
    void loadTreeSettings();
    void check();
    void removeLast();
  private:
    QTreeWidget* _twDevices;
    DeviceListWidget* _deviceWidgetList;
    std::shared_ptr<QButtonGroup> _buttonGroup ;
//    ShPtrClientManager _clientManager;
//    QListWidget* _lwDeviceWidgets;
    /*
        std::map<QTreeWidgetItem*,
            QListWidgetItem*> _devicesesT;

        std::map<QTreeWidgetItem*,
            std::pair<QListWidgetItem*, ShPtrDeviceClient>> _devicesesTree;
        std::map<QListWidgetItem*,
            std::shared_ptr<DeviceSetClient> > _devicesesList;
            */

};
///@}
#endif // TREE_DEVICES_H
