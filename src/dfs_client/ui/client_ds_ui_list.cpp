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
            this, &DeviceSetListWidget::onStationItemSelected);

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
        this->onRemoveDeviceSetWidget();
    });

    loadSettings();
}

DeviceSetListWidget::~DeviceSetListWidget()
{
    qDebug() << "DESTR_BEGIN";
    saveSettings();
    qDebug() << "DESTR_END";
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
    QList<QListWidgetItem*> itemList = _listWidget->selectedItems();
    for(QListWidgetItem* item : itemList)
    {
        DeviceSetWidget* widget = qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget)
        {
            _counter++;
            widget->sendCommand(command);
        }
    }
}


QList<DeviceSetWidget*> DeviceSetListWidget::createdDeviceSetWidgets()
{
    QList<QListWidgetItem*> itemList = _listWidget->selectedItems();
    QList<DeviceSetWidget*> devieceWidgets;
    qDebug() << "ConnectToSelection" << itemList.size();
    for(QListWidgetItem* item : itemList)
    {
        if(DeviceSetWidget* widget =
                    qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item)))
        {
            devieceWidgets.append(widget);
        }
    }
    return devieceWidgets;
}



void DeviceSetListWidget::addDeviceSetWidget(DeviceSetWidget* deviceSetWidget)
{
    connect(deviceSetWidget, &DeviceSetWidget::commandSuccessed,
            [this]
    {
        if(_commandQueue.isEmpty()) return ;
        if(--_counter != 0) return ;
        const proto::receiver::Command& successedCommand = _commandQueue.dequeue();

        if(successedCommand.command_type() == proto::receiver::START_DDC1)
        {
            emit  ready(_connectedDeviceSetWidgetList);
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

    QListWidgetItem* stationPanelItem = new QListWidgetItem(_listWidget);

    stationPanelItem->setSizeHint(deviceSetWidget->sizeHint());
    stationPanelItem->setSelected(true);
    _listWidget->setItemWidget(stationPanelItem, deviceSetWidget);
    DeviceSetWidget* dsw =
        _listWidget->findChild<DeviceSetWidget*>(deviceSetWidget->objectName());
    _allDeviceSetWidgetList.append(deviceSetWidget);
    if(dsw)
    {
        qDebug() << "FIND CHILD";
    }
    else
    {
        qDebug() << "NOT FIND CHILD";
    }
}

void DeviceSetListWidget::removeDeviceSetWidget(DeviceSetWidget* deviceSetWidget)
{
    //TODO ИСПОЛЬЗОВАТЬ МЕТОДЫ ПОИСКА ВСТРИННЫЕ В QT - FIND (wid);
    DeviceSetWidget* dsw =
        _listWidget->findChild<DeviceSetWidget*>(deviceSetWidget->objectName());
    if(dsw)
    {
        qDebug() << "FIND CHILD";
    }
    else
    {
        qDebug() << "NOT FIND CHILD";
    }

    for(int i = 0; i < _listWidget->count(); i++)
    {
        QListWidgetItem* selectedItem = _listWidget->item(i);
        DeviceSetWidget* currentPanel = qobject_cast<DeviceSetWidget*>
                                        (_listWidget->itemWidget(selectedItem));
        if(currentPanel == deviceSetWidget)
        {
            _listWidget->takeItem(_listWidget->row(selectedItem));
            _listWidget->removeItemWidget(selectedItem);
            _allDeviceSetWidgetList.removeOne(deviceSetWidget);
            break;
        }
    }
}

void DeviceSetListWidget::connectToSelectedDeviceSet()
{
    _connectedDeviceSetWidgetList.clear();

    QList<QListWidgetItem*> itemList = _listWidget->selectedItems();
    qDebug() << "ConnectToSelection" << itemList.size();
    for(QListWidgetItem* item : itemList)
    {
        DeviceSetWidget* widget = qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget)
        {
            widget->connectToDeviceSet();
            _connectedDeviceSetWidgetList.append(widget);
        }
        else
        {
            qDebug() << "NULL DEVICE SET WIDGET";
        }

    }
    setCursor(Qt::WaitCursor);

    checkingConnections(true);
}

void DeviceSetListWidget::disconnectFromSelectedDeviceSet()
{
    QList<QListWidgetItem*> itemList = _listWidget->selectedItems();
    qDebug() << "ConnectToSelection" << itemList.size();
    for(QListWidgetItem* item : itemList)
    {
        DeviceSetWidget* widget = qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget)
        {
            widget->disconnectFromDeviceSet();
        }
        else
        {
            qDebug() << "NULL DEVICE SET WIDGET";
        }

    }
    setCursor(Qt::WaitCursor);
    checkingConnections(false);
}

void DeviceSetListWidget::checkingConnections(bool checkingState)
{
    std::shared_ptr<QTimer> shpTimer = std::make_shared<QTimer>();
    std::shared_ptr<QTime> shpTime = std::make_shared<QTime>();
    std::shared_ptr<QMetaObject::Connection>shpConnection =
        std::make_shared<QMetaObject::Connection>();
    shpTime->start();

    *shpConnection = connect(shpTimer.get(), &QTimer::timeout,
                             [this, checkingState, shpTimer, shpTime, shpConnection]
    {
        bool success = false;
        for (DeviceSetWidget* deviceSetWidget : _connectedDeviceSetWidgetList)
            if(checkingState == deviceSetWidget->isConnected())
            {
                success = true;
            }
            else
            {
                success = false;
                break;
            }

        if(!success && shpTime->elapsed() >= TIME_WAIT_CONNECTION)
        {
            QObject::disconnect(*shpConnection);
            setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this, "Connection Failed", "");
        }
        else if(success)
        {
            QObject::disconnect(*shpConnection);
            setCursor(Qt::ArrowCursor);
        }
    });
    shpTimer->start(500);
}

void DeviceSetListWidget::setCursor(const QCursor& cursor)
{
    //TODO НЕПОНЯТНО ПОЧЕМУ ДВА PARENT WIDGET НУЖНО СТАВИТЬ
    if(parentWidget())
    {
        parentWidget()->parentWidget()->setCursor(cursor);
    }
}

void DeviceSetListWidget::onRemoveDeviceSetWidget()
{
    QList<QListWidgetItem*> selectedItems = _listWidget->selectedItems();
    for(QListWidgetItem* selectedItem : selectedItems)
    {
        if(DeviceSetWidget* widget = qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(selectedItem)))
        {
            _listWidget->removeItemWidget(selectedItem);
            _allDeviceSetWidgetList.removeOne(widget);
            delete _listWidget->takeItem(_listWidget->row(selectedItem));
        }
    }
}

void DeviceSetListWidget::onStationItemSelected(QListWidgetItem* item)
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

void DeviceSetListWidget::onAddDeviceSetWidget()
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
        this->addDeviceSetWidget(new DeviceSetWidget(address, port));
    }
    else   QMessageBox::
        warning(this, "Creation Device Set Client faild", "address:port");

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
        quint16 groupCounter = 1;
        for(const auto& group : groups)
        {
            s.beginGroup(group);

            QString ip = s.value("ip").toString();
            quint16 port = static_cast<quint16>(s.value("port").toUInt());

            DeviceSetWidget* widget = new DeviceSetWidget(ip, port);
            widget->setListeningStreamPort(PORT + groupCounter);
            addDeviceSetWidget(widget);

            s.endGroup();
            ++groupCounter;
        }
    }
}

void DeviceSetListWidget::saveSettings()
{
    QString prefix = "device";

    QString  settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_FILE;
//    qDebug()<<"DeviceSetListWidget::saveSettings()"<<settingsFileName<<_listWidget->count();

    QSettings s(settingsFileName, QSettings::IniFormat);
    s.clear();
//    if(QFile::exists(settingsFileName))
//    {
    qDebug() << "SIZE:" << _allDeviceSetWidgetList.size();
    for(int i = 0; i < _allDeviceSetWidgetList.size(); ++i)
    {

        s.beginGroup(prefix + "_" + QString::number(i + 1));

        s.setValue("ip", _allDeviceSetWidgetList[i]->address());
        s.setValue("port", _allDeviceSetWidgetList[i]->port());

        s.endGroup();
        qDebug() << "Dws" << _allDeviceSetWidgetList[i];
    }
    s.sync();
//    }
}
