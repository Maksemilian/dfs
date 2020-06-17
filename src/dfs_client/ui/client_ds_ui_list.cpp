#include "client_ds_ui_list.h"

#include "client_ds_ui.h"

#include "tool_switch_button.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QSettings>

///@{

const QString DeviceListWidget::SETTINGS_FILE = "client_device_set_list.ini";

const QString DeviceListWidget::STRING_CONNECT = "connect";
const QString DeviceListWidget::STRING_DISCONNECT = "disconnect";

DeviceListWidget::DeviceListWidget(const std::shared_ptr<ClientManager>& clientManager,
                                   QWidget* parent)
    : QWidget(parent), _listWidget(new QListWidget(this)), _clientManager(clientManager)
{
    setLayout(new QVBoxLayout);
    _listWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    connect(_listWidget, &QListWidget::itemClicked,
            [](QListWidgetItem * item)
    {
        if(!item->isSelected())
        {
            item->setSelected(true);
        }
    });
    SwitchButton* pbConnectToStation = new SwitchButton(STRING_CONNECT, STRING_DISCONNECT,
            false, this);

    layout()->addWidget(_listWidget);
    layout()->addWidget(pbConnectToStation);

    connect(pbConnectToStation, &SwitchButton::stateChanged,
            [this](bool state)
    {
        if(state)
        {
            qDebug() << "Start Clients:" ;
            _clientManager->startClients();
            emit clieckedConnectedButton();
        }
        else
        {
            qDebug() << "Stop Clients:" ;
            _clientManager->stopClients();
            emit clieckedDisconnectedButton();
        }
    });

    loadSettings();
}

void DeviceListWidget::clear()
{
    _clientManager->clients();
    _deviceses.clear();
    _listWidget->clear();
}

DeviceListWidget::~DeviceListWidget()
{
    saveSettings();
}

void DeviceListWidget::addDeviceWidget(const ConnectData& connectData,
                                       DeviceWidget* deviceSetWidget)
{
    QListWidgetItem* deviceItem = new QListWidgetItem(_listWidget);

    deviceItem->setSizeHint(deviceSetWidget->sizeHint());
    deviceItem->setSelected(true);
    _listWidget->setItemWidget(deviceItem, deviceSetWidget);

    std::shared_ptr<DeviceClient> deviceSetClient =
        std::make_shared<DeviceClient>(connectData);

    connect(deviceSetClient.get(), &DeviceClient::deviceReady,
            deviceSetWidget, &DeviceWidget::onDeviceOpen);

    connect(deviceSetClient.get(), &DeviceClient::stoped,
            deviceSetWidget, &DeviceWidget::onDeviceClose);

    connect(deviceSetClient.get(), &DeviceClient::commandFailed,
            deviceSetWidget, &DeviceWidget::onShowDeviceError);

    _deviceses[deviceItem] = deviceSetClient;
    _clientManager->addClient(deviceSetClient);
}

void DeviceListWidget::removeDeviceWidget(DeviceWidget* removedWidget)
{
    for (auto& device : _deviceses)
    {
        if(DeviceWidget* currentWidget = qobject_cast<DeviceWidget*>
                                         (_listWidget->itemWidget(device.first)))
        {
            if(removedWidget == currentWidget)
            {
                _deviceses.erase(device.first);
                _clientManager->removeClient(device.second);
                _listWidget->removeItemWidget(device.first);
                delete _listWidget->takeItem(_listWidget->row(device.first));
                break;
            }
        }
    }
}

//************** LOAD\SAVE SETTINGS

void DeviceListWidget::loadSettings()
{
    QString settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_FILE;
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
}

void DeviceListWidget::saveSettings()
{
    QSettings s( QApplication::applicationDirPath() + "/" + SETTINGS_FILE,
                 QSettings::IniFormat);
    s.clear();

    QString prefix = "device";
    int deviceNumber = 0;
    for(const auto& device : _deviceses)
    {
        if(DeviceWidget* widget =
                    qobject_cast<DeviceWidget*>(_listWidget->itemWidget(device.first)))
        {
            s.beginGroup(prefix + "_" + QString::number(deviceNumber + 1));

            const ConnectData& connectData = device.second->connectData();
            s.setValue("name", widget->getName());
            s.setValue("address", connectData.address);
            s.setValue("port", connectData.port);
            s.endGroup();
            ++deviceNumber;
        }
    }
    s.sync();
}

///@}
