#include "tree_devices.h"
#include "tree_pc_item.h"

#include "client_ds_ui_list.h"
#include "client_ds_ui.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMessageBox>
#include <QKeyEvent>
#include <QHeaderView>

/*! \addtogroup client
 */
///@{
const QString DeviceMonitoring::SETTINGS_TREE_FILE_NAME = "tree_devices.ini";
const QString DeviceMonitoring::SETTINGS_LIST_FILE_NAME = "client_device_set_list.ini";

DeviceMonitoring::DeviceMonitoring(
    const ShPtrClientManager& clientManager,
    QWidget* parent):
    QSplitter (Qt::Horizontal, parent),
//    _clientManager(clientManager),
    _twDevices(new QTreeWidget(this)),
    _deviceWidgetList(new DeviceListWidget(clientManager, this)),
    _buttonGroup( std::make_shared<QButtonGroup>())
{
    _twDevices->setColumnCount(2);
    _twDevices->headerItem()->setHidden(true);
//    _twDevices->header()->setResizeContentsPrecision(QHeaderView::ResizeToContents);
    _twDevices->setColumnWidth(0, 100);
    _twDevices->setAutoFillBackground(true);
    _twDevices->setSelectionMode(QAbstractItemView::MultiSelection);
    _twDevices->setSelectionBehavior(QAbstractItemView::SelectItems);

    addWidget(_twDevices);
    addWidget(_deviceWidgetList);

    _twDevices->installEventFilter(this);

    _buttonGroup->setExclusive(true);
    connect(_deviceWidgetList, &DeviceListWidget::clieckedConnectedButton,
            [this]()
    {
        QList<int> sizesW = sizes();
        sizesW.replace(0, 0);
        sizesW.replace(1, this->width() / 2);
        setSizes(sizesW);

    });
    connect(_deviceWidgetList, &DeviceListWidget::clieckedDisconnectedButton,
            [this]()
    {
        QList<int> sizesW = sizes();
        sizesW.replace(0, this->width() / 2);
        sizesW.replace(1, this->width() / 2);
        setSizes(sizesW);

    });
    connect(_twDevices, &QTreeWidget::itemClicked,
            [this](QTreeWidgetItem * item, int column)
    {

        if(column == 0)
        {
            if((item->data(column, Qt::UserRole) ==
                    TreePcWidgetItem::DT_DESCTOP ||
                    item->data(column, Qt::UserRole) ==
                    TreePcWidgetItem::DT_RECEIVER)
                    && item->isSelected())
            {
                item->setSelected(false);
                return ;
            }

            if(item->isSelected())
            {
                QRadioButton* radioButton = new QRadioButton(this);
                _buttonGroup->addButton(radioButton);
                _twDevices->setItemWidget(item, 1, radioButton);

                check();
            }
            else
            {
//                qDebug() << "RemoveItem" << item;
                _twDevices->removeItemWidget(item, 1);
            }
        }
    }
           );

    loadTreeSettings();
}

void DeviceMonitoring::check()
{
    if(_twDevices->selectedItems().size() ==
            MAX_COUNT_SELECTED_DEVICE_SET &&
            _buttonGroup->checkedButton())
    {
        QMessageBox::StandardButton stanartButton =
            QMessageBox::question(this, "TW",
                                  "Create set for selected devices ?");
        if(stanartButton == QMessageBox::Yes)
        {
            createDeivceWidgets();
        }
        else
        {
            removeLast();
        }
    }
    else  if(_twDevices->selectedItems().size() >
             MAX_COUNT_SELECTED_DEVICE_SET)
    {
        QMessageBox::information(this, "I", "Only " +
                                 QString::number(MAX_COUNT_SELECTED_DEVICE_SET)
                                 + "device can be used");
        removeLast();
    }
}

void DeviceMonitoring::removeLast()
{
    QTreeWidgetItem* item = _twDevices->selectedItems().last();
    item->setSelected(false);
    _twDevices->removeItemWidget(item, 1);
    qDebug() << "TW" << _twDevices->selectedItems().size();

}

DeviceMonitoring::~DeviceMonitoring()
{
    saveTreeSettings();
}

bool DeviceMonitoring::eventFilter(QObject* obj, QEvent* event)
{
    if( event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(obj == _twDevices && keyEvent->key() == Qt::Key_Space)
        {
            check();
        }
        return true;
    }

    return  QObject::eventFilter(obj, event);
}



void DeviceMonitoring::createDeivceWidgets()
{
    QList<QTreeWidgetItem*> treeItems = _twDevices->selectedItems();
    _deviceWidgetList->clear();
    for(auto deviceTreeItem : treeItems)
    {
        QTreeWidgetItem* parent = deviceTreeItem->parent();

        QString text = deviceTreeItem->parent()->text(0);
        QStringList stringList = text.split(":");

        QString address = stringList[0];
        QString port = stringList[1];

        int index = parent->indexOfChild(deviceTreeItem);
        bool flag = deviceTreeItem->data(0, Qt::UserRole + 1).toBool();
//        qDebug() << address << "DS_" << index << flag;

        ConnectData connectData = {address,
                                   port.toUShort(),
                                   SessionType::SESSION_COMMAND
                                  };
        _deviceWidgetList->addDeviceWidget(connectData,
                                           new DeviceWidget(address +
                                                   "DS_" +
                                                   QString::number(index)));

    }
}

void DeviceMonitoring::saveTreeSettings()
{
    QSettings settings(QApplication::applicationDirPath() + "/" + SETTINGS_TREE_FILE_NAME, QSettings::IniFormat);
    QString prefix = "PC_";
    for (int i = 0; i < _twDevices->topLevelItemCount(); ++i)
    {
        if(TreePcWidgetItem* item =
                    dynamic_cast<TreePcWidgetItem*>(_twDevices->topLevelItem(i)))

        {
            settings.beginGroup(prefix + QString::number(i));
            settings.setValue("ip", item->getIpAddress());
            settings.setValue("port", item->getPort());
            settings.endGroup();
        }
    }
}

void DeviceMonitoring::loadTreeSettings()
{
    QSettings s(QApplication::applicationDirPath() + "/" + SETTINGS_TREE_FILE_NAME, QSettings::IniFormat);
    QStringList groups = s.childGroups();
    for(const auto& group : groups)
    {
        s.beginGroup(group);

        QString ip = s.value("ip").toString();
        QString port = s.value("port").toString();

        _twDevices->addTopLevelItem(new TreePcWidgetItem(ip, port, _twDevices));

        s.endGroup();
    }
}
///@}
/*
void DeviceMonitoring::loadListSettings()
{
    QString settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_LIST_FILE_NAME;
    //    qDebug()<<"DeviceSetListWidget::loadSettings()"<<settingsFileName;

    QSettings s(settingsFileName, QSettings::IniFormat);

    QStringList groups = s.childGroups();
    for(const auto& group : groups)
    {
        s.beginGroup(group);
        QString name = s.value("name").toString();
        QString address = s.value("address").toString();
        quint16 port = static_cast<quint16>(s.value("port").toUInt());

        ConnectData connectData = {address, port, SessionType::SESSION_COMMAND};
        addDeviceWidget(connectData, new DeviceWidget(name));

        s.endGroup();
    }
    //TODO WARNING
    startClients();

}

void DeviceMonitoring::saveListSettings()
{
    QSettings s( QApplication::applicationDirPath() + "/" + SETTINGS_LIST_FILE_NAME,
                 QSettings::IniFormat);
    s.clear();

    QString prefix = "device";
    int deviceNumber = 0;
    for(const auto& device : _devicesesT)
    {
        if(DeviceWidget* widget =
                    qobject_cast<DeviceWidget*>(_lwDeviceWidgets->itemWidget(device.second)))
        {
            s.beginGroup(prefix + "_" + QString::number(deviceNumber + 1));

            QTreeWidgetItem* connectData = device.first;
            auto list = connectData->parent()->text(0).split(":");
//            qDebug() << widget->getName() << list;
            s.setValue("name", widget->getName());
            s.setValue("address", list[0]);
            s.setValue("port", list[1]);
            s.endGroup();
            ++deviceNumber;
        }
    }
    s.sync();
}

void DeviceMonitoring::addDeviceWidget(const ConnectData& connectData,
                                       DeviceWidget* deviceSetWidget)
{
    QListWidgetItem* deviceItem = new QListWidgetItem(_lwDeviceWidgets);

    deviceItem->setSizeHint(deviceSetWidget->sizeHint());
//    deviceItem->setSelected(true);
    _lwDeviceWidgets->setItemWidget(deviceItem, deviceSetWidget);

    std::shared_ptr<DeviceSetClient> deviceSetClient =
        std::make_shared<DeviceSetClient>(connectData);

    connect(deviceSetClient.get(), &DeviceSetClient::deviceReady,
            deviceSetWidget, &DeviceWidget::onDeviceOpen);

    connect(deviceSetClient.get(), &DeviceSetClient::stoped,
            deviceSetWidget, &DeviceWidget::onDeviceClose);

    connect(deviceSetClient.get(), &DeviceSetClient::commandFailed,
            deviceSetWidget, &DeviceWidget::onShowDeviceError);

    _devicesesList[deviceItem] = deviceSetClient;

    _clientManager->addClient(deviceSetClient);

}*
/
//                connect(radioButton, &QRadioButton::toggled,
//                        [item, this](bool state)
//                {
//                    qDebug() << "Item For Button" << item;
//                    item->setData(0, Qt::UserRole + 1, state);
//                    if(_twDevices->selectedItems().size() ==
//                            MAX_COUNT_SELECTED_DEVICE_SET)
//                    {
//                        check();
//                    }
//                });
/*
if(selItems.size() == MAX_COUNT_SELECTED_DEVICE_SET)
{
    for (auto item : selItems)
    {
        if(item->data(0, Qt::UserRole + 1).toBool())
        {
//                _pbCreateSet->setEnabled(true);
            return;
        }
        else
        {
            qDebug() << "User Role:" << item->data(0, Qt::UserRole + 1).toBool();
        }
    }
}
else if(selItems.size() > MAX_COUNT_SELECTED_DEVICE_SET)
{
//        _pbCreateSet->setEnabled(false);
//        QMessageBox::information(this, "Warning", "selected > 3 Devices");
//        selItems.last()->setSelected(false);
}*/

/*
QPushButton* pbAddDesctopItem = new QPushButton("Add PC", this);
QPushButton* pbCreateSet = new QPushButton("Create Set");

pbCreateSet->setEnabled(false);

connect(pbCreateSet, &QPushButton::clicked,
        this, &TreeDevices::createDeivceWidgets);

connect(pbAddDesctopItem, &QPushButton::clicked,
        [this]
{
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal);
    QStringList stringList = text.split(":");
    TreePcWidgetItem* item =  new TreePcWidgetItem(stringList[0], stringList[1], _twDevices);

    _twDevices->addTopLevelItem(item);
});
*/
