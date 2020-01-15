#include "client_ds_ui_list.h"

#include "client_ds_ui.h"

#include "tool_switch_button.h"
#include "i_device_settings.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTimer>
#include <QTime>
#include <QMessageBox>
#include <QLabel>
#include <QInputDialog>

const QString DeviceSetListWidget::SETTINGS_FILE = "client_device_set_list.ini";
//const QString DeviceSetListWidget::SETTINGS_FILE="ds_list.ini";

const QString DeviceSetListWidget::STRING_CONNECT = "connect";
const QString DeviceSetListWidget::STRING_DISCONNECT = "disconnect";

class DeviceWidgetCreator: public QDialog
{
  public:
    DeviceWidgetCreator(QWidget* parent = nullptr):
        QDialog(parent),
        _leIpAddress(new QLineEdit),
        _lePort(new QLineEdit)
    {
        QGridLayout* gl = new QGridLayout;
        gl->addWidget(new QLabel("Ip:", this), 0, 0);
        gl->addWidget(_leIpAddress, 0, 1);

        gl->addWidget(new QLabel("Port:", this), 1, 0);
        gl->addWidget(_lePort, 1, 1);
    }
    virtual ~DeviceWidgetCreator() override {}
  private:
    QLineEdit* _leIpAddress;
    QLineEdit* _lePort;
};

DeviceSetListWidget::DeviceSetListWidget(QWidget* parent)
    : QWidget(parent), _listWidget(new QListWidget(this))
{
    setLayout(new QVBoxLayout);
    _listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    _pbConnectToStation = new SwitchButton(STRING_CONNECT, STRING_DISCONNECT,
                                           false, this);

    //    _pbConnectToStation->setCurrentState(false);

    layout()->addWidget(_listWidget);
    layout()->addWidget(_pbConnectToStation);

    connect(_listWidget, &QListWidget::itemClicked,
            this, &DeviceSetListWidget::onDeviceSelected);

    connect(_pbConnectToStation, &SwitchButton::stateChanged,
            this, &DeviceSetListWidget::onTest);

    connect(this, &DeviceSetListWidget::allConnectedState,
            _pbConnectToStation, &SwitchButton::setCurrentState);
    QPushButton* pbAddDeviceWidget = new QPushButton("Add");
    QPushButton* pbRemoveDeviceWidget = new QPushButton("Remove");

    layout()->addWidget(pbAddDeviceWidget);
    layout()->addWidget(pbRemoveDeviceWidget);
    connect(pbAddDeviceWidget, &QPushButton::clicked,
            [this]
    {
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("User name:"), QLineEdit::Normal);
        QStringList stringList = text.split(":");

        this->addDeviceSetWidget(new  DeviceSetWidget(stringList[0], stringList[1].toUShort()));
    });

    connect(pbRemoveDeviceWidget, &QPushButton::clicked,
            [this]
    {
        this->onRemoveDevice();
    });

    loadSettings();
}

DeviceSetListWidget::~DeviceSetListWidget()
{
    qDebug() << "DESTR_BEGIN";
    saveSettings();
    qDebug() << "DESTR_END";
}

void DeviceSetListWidget::setCursor(const QCursor& cursor)
{
    //TODO НЕПОНЯТНО ПОЧЕМУ ДВА PARENT WIDGET НУЖНО СТАВИТЬ
    if(parentWidget())
    {
        parentWidget()->parentWidget()->setCursor(cursor);
    }
}

void DeviceSetListWidget::onDeviceSetUpdate(quint32 numberDeviceSet)
{
    DeviceSetWidget* deviceSetWidget = qobject_cast<DeviceSetWidget*>(sender());
    QList<QListWidgetItem*> items = _listWidget->selectedItems();
    for (auto& device : items)
    {
        if(qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(device)) == deviceSetWidget)
        {
            proto::receiver::Command command;
            command.set_command_type(proto::receiver::SET_DEVICE_INDEX);
            command.set_device_set_index(numberDeviceSet);
            command.set_demo_mode(false);
            _deviceses[device]->sendCommand(command);
            break;
        }
        else
        {
            qDebug() << "BAD CAST:" << deviceSetWidget
                     << qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(device));
        }
    }
}


void DeviceSetListWidget::addDeviceSetWidget(DeviceSetWidget* deviceSetWidget)
{
    QListWidgetItem* deviceItem = new QListWidgetItem(_listWidget);

    deviceItem->setSizeHint(deviceSetWidget->sizeHint());
    deviceItem->setSelected(true);
    _listWidget->setItemWidget(deviceItem, deviceSetWidget);

    DeviceSetClient* deviceSetClient = new DeviceSetClient;
    connect(deviceSetClient, &DeviceSetClient::commandSuccessed,
            [this]
    {
        if(_commandQueue.isEmpty()) return ;
        if(--_counter != 0) return ;
        const proto::receiver::Command& successedCommand = _commandQueue.dequeue();

        if(successedCommand.command_type() == proto::receiver::START_DDC1)
        {
            std::vector<ShPtrPacketBuffer> buffers;
            for(auto& device : _deviceses)
            {
                buffers.push_back(device.second->getDDC1Buffer());
            }
//            emit  ready(_connectedDeviceSetWidgetList);
            emit readyTest(buffers);
        }
        else if(successedCommand.command_type() == proto::receiver::STOP_DDC1)
        {
            emit notReady();
        }

        if(!_commandQueue.isEmpty() &&
                _commandQueue.head().command_type() != successedCommand.command_type())
        {
            setAllDeviceSet(_commandQueue.head());
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    });

    connect(deviceSetWidget, &DeviceSetWidget::getDeviceInfoUpdate,
            this, &DeviceSetListWidget::onDeviceSetUpdate);

    connect(deviceSetClient, &DeviceSetClient::deviceSetReady,
            deviceSetWidget, &DeviceSetWidget::onDeviceSetReady);

    connect(deviceSetClient, &DeviceSetClient::deviceInfoUpdated,
            deviceSetWidget, &DeviceSetWidget::onDeviceSetInfoUpdate);

    connect(deviceSetClient, &DeviceSetClient::disconnected,
            deviceSetWidget, &DeviceSetWidget::onDeviceSetDisconnected);

    connect(deviceSetClient, &DeviceSetClient::commandFailed,
            deviceSetWidget, &DeviceSetWidget::onDeviceSetCommandFailed);

    _deviceses[deviceItem].reset(deviceSetClient);
    deviceSetClient->setLiceningStreamPort(PORT + static_cast<quint16>(_deviceses.size()));

}

void DeviceSetListWidget::removeDeviceSetWidget(DeviceSetWidget* removedWidget)
{
    for (auto& device : _deviceses)
    {
        if(DeviceSetWidget* currentWidget = qobject_cast<DeviceSetWidget*>
                                            (_listWidget->itemWidget(device.first)))
        {
            if(removedWidget == currentWidget)
            {
                _deviceses.erase(device.first);

                _listWidget->removeItemWidget(device.first);
                delete _listWidget->takeItem(_listWidget->row(device.first));
                break;
            }
        }
    }

}

void DeviceSetListWidget::connectToSelectedDeviceSet()
{
    QList<QListWidgetItem*> items = _listWidget->selectedItems();

    qDebug() << "ConnectToSelection" << items.size();
    for(auto& item : items)
    {
        if(DeviceSetWidget* widget =
                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item)))
        {
            _deviceses[item]->connectToHost(widget->address(), widget->port(), SessionType::SESSION_COMMAND);
        }
        else
        {
            qDebug() << "NULL DEVICE SET WIDGET";
        }
    }

//    setCursor(Qt::WaitCursor);

//    checkingConnections(true);

}

void DeviceSetListWidget::disconnectFromSelectedDeviceSet()
{
    QList<QListWidgetItem*> items = _listWidget->selectedItems();

    qDebug() << "ConnectToSelection" << items.size();
    for(auto& item : items)
    {
        if(DeviceSetWidget* widget =
                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item)))
        {
            _deviceses[item]->disconnectFromHost();
        }
        else
        {
            qDebug() << "NULL DEVICE SET WIDGET";
        }
    }

//    setCursor(Qt::WaitCursor);

}

void DeviceSetListWidget::setCommand(proto::receiver::Command& command)
{
    setCursor(Qt::WaitCursor);

    if(_commandQueue.isEmpty())
    {
        setAllDeviceSet(command);
    }

    _commandQueue.enqueue(command);
}

void DeviceSetListWidget::setAllDeviceSet( proto::receiver::Command& command)
{
    QList<QListWidgetItem*> items = _listWidget->selectedItems();

    for(auto& item : items)
    {
        _deviceses[item]->sendCommand(command);
        _counter++;
    }
}

void DeviceSetListWidget::onAddDevice()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Creation Device Set Client",
                                         "Ip address:port",
                                         QLineEdit::Normal,
                                         "address:port", &ok);
    QStringList list = text.split(":");


    QString address = list[0];
    quint16 port = list[1].toUShort();

    if(ok && port != 0)
    {
        addDeviceSetWidget(new DeviceSetWidget(address, port));
    }
    else   QMessageBox::
        warning(this, "Creation Device Set Client faild", "address:port");

}

void DeviceSetListWidget::onRemoveDevice()
{
    QList<QListWidgetItem*> selectedItems = _listWidget->selectedItems();
    for(QListWidgetItem* selectedItem : selectedItems)
    {
        if(DeviceSetWidget* widget = qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(selectedItem)))
        {
            removeDeviceSetWidget(widget);
        }
    }
}

void DeviceSetListWidget::onDeviceSelected(QListWidgetItem* item)
{
    Q_UNUSED(item);
    qDebug() << "Station Panel Clicked";
    if(_listWidget->selectedItems().isEmpty())
    {
        _pbConnectToStation->setEnabled(false);
    }
    else
    {
        _pbConnectToStation->setEnabled(true);
    }
}

//************** LOAD\SAVE SETTINGS

void DeviceSetListWidget::loadSettings()
{
    QString settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_FILE;
    //    qDebug()<<"DeviceSetListWidget::loadSettings()"<<settingsFileName;

    if(QFile::exists(settingsFileName))
    {

        QSettings s(settingsFileName, QSettings::IniFormat);
        QStringList groups = s.childGroups();
        for(const auto& group : groups)
        {
            s.beginGroup(group);

            QString ip = s.value("ip").toString();
            quint16 port = static_cast<quint16>(s.value("port").toUInt());

            addDeviceSetWidget( new DeviceSetWidget(ip, port));

            s.endGroup();
        }
    }
}

void DeviceSetListWidget::saveSettings()
{
    QSettings s( QApplication::applicationDirPath() + "/" + SETTINGS_FILE,
                 QSettings::IniFormat);
    s.clear();

    QString prefix = "device";
    int deviceNumber = 0;
    for(const auto& device : _deviceses)
    {
        if(DeviceSetWidget* widget =
                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(device.first)))
        {
            s.beginGroup(prefix + "_" + QString::number(deviceNumber + 1));

            s.setValue("ip", widget->address());
            s.setValue("port", widget->port());

            s.endGroup();
            ++deviceNumber;
        }
    }
    s.sync();
}

void DeviceSetListWidget::onTest(bool state)
{
    qDebug() << "State:" << state;
    if(state)
    {
        connectToSelectedDeviceSet();
    }
    else
    {
        disconnectFromSelectedDeviceSet();
    }
}


//void DeviceSetListWidget::checkingConnections(bool checkingState)
//{
//    std::shared_ptr<QTimer> shpTimer = std::make_shared<QTimer>();
//    std::shared_ptr<QTime> shpTime = std::make_shared<QTime>();
//    std::shared_ptr<QMetaObject::Connection>shpConnection =
//        std::make_shared<QMetaObject::Connection>();
//    shpTime->start();

//    *shpConnection = connect(shpTimer.get(), &QTimer::timeout,
//                             [this, checkingState, shpTimer, shpTime, shpConnection]
//    {
//        bool success = false;

//        for(const auto& device : _deviceses)
//        {
//            if(DeviceSetWidget* deviceSetWidget =
//                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(device.first)))
//            {
//                if(checkingState == deviceSetWidget->isConnected())
//                {
//                    success = true;
//                }
//                else
//                {
//                    success = false;
//                    break;
//                }
//            }
//        }

//        if(!success && shpTime->elapsed() >= TIME_WAIT_CONNECTION)
//        {
//            QObject::disconnect(*shpConnection);
//            setCursor(Qt::ArrowCursor);
//            QMessageBox::warning(this, "Connection Failed", "");
//        }
//        else if(success)
//        {
//            QObject::disconnect(*shpConnection);
//            setCursor(Qt::ArrowCursor);
//        }
//    });

//    shpTimer->start(500);
//}
//QList<DeviceSetWidget*> DeviceSetListWidget::createdDeviceSetWidgets()
//{
//    QList<QListWidgetItem*> itemList = _listWidget->selectedItems();
//    QList<DeviceSetWidget*> devieceWidgets;
//    qDebug() << "ConnectToSelection" << itemList.size();
//    for(QListWidgetItem* item : itemList)
//    {
//        if(DeviceSetWidget* widget =
//                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item)))
//        {
//            devieceWidgets.append(widget);
//        }
//    }
//    return devieceWidgets;
//}
