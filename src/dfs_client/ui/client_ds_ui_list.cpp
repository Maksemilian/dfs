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

const QString DeviceSetListWidget::SETTINGS_FILE="client_device_set_list.ini";

const QString DeviceSetListWidget::STRING_CONNECT="connect";
const QString DeviceSetListWidget::STRING_DISCONNECT="disconnect";

DeviceSetListWidget::DeviceSetListWidget(QWidget *parent)
    :QWidget(parent),_listWidget(new QListWidget(this))
{
    setLayout(new QVBoxLayout);
    _listWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    _pbConnectToStation=new SwitchButton(STRING_CONNECT,STRING_DISCONNECT,
                                         false,this);

    //    _pbConnectToStation->setCurrentState(false);

    layout()->addWidget(_listWidget);
    layout()->addWidget(_pbConnectToStation);

    connect(_listWidget,&QListWidget::itemClicked,
            this,&DeviceSetListWidget::onStationItemSelected);

    connect(_pbConnectToStation,&SwitchButton::stateChanged,
            this,&DeviceSetListWidget::onTest);

    connect(this,&DeviceSetListWidget::allConnectedState,
            _pbConnectToStation,&SwitchButton::setCurrentState);

    createDevieSetWidgets();
}

void DeviceSetListWidget::onTest(bool state)
{
    qDebug()<<"State:"<<state;
    if(state){
        connectToSelectedDeviceSet();
    }else {
        disconnectFromSelectedDeviceSet();
    }
}

void DeviceSetListWidget::setCommand(const proto::receiver::Command &command)
{
    setCursor(Qt::WaitCursor);

    if(_commandQueue.isEmpty())
        setAllDeviceSet(command);

    _commandQueue.enqueue(command);
}

void DeviceSetListWidget::setAllDeviceSet(const proto::receiver::Command &command)
{
    QList<QListWidgetItem*> itemList=_listWidget->selectedItems();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget){
            _counter++;
            widget->sendCommand(command);
        }
    }
}

void DeviceSetListWidget::createDevieSetWidgets()
{
    QString settingsFileName=QApplication::applicationDirPath()+"/"+SETTINGS_FILE;
    if(QFile::exists(settingsFileName)){
        QString ip;
        quint16 port;
        QSettings s(settingsFileName,QSettings::IniFormat);
        s.beginGroup("dev_1");
        ip=s.value("ip").toString();
        port=static_cast<quint16>(s.value("port").toUInt());
        s.endGroup();
        qDebug()<<"DeviceSet:"<<ip<<" "<<port;
        DeviceSetWidget *widget=new DeviceSetWidget(ip,port);
        widget->setListeningStreamPort(9001);
        addDeviceSetWidget(new DeviceSetWidget(ip,port));

//        s.beginGroup("dev_2");
//        ip=s.value("ip").toString();
//        port=static_cast<quint16>(s.value("port").toUInt());
//        s.endGroup();
//        qDebug()<<"DeviceSet:"<<ip<<" "<<port;
//        addDeviceSetWidget(new DeviceSetWidget(ip,port));

//        s.beginGroup("dev_3");
//        ip=s.value("ip").toString();
//        port=static_cast<quint16>(s.value("port").toUInt());
//        s.endGroup();
//        qDebug()<<"DeviceSet:"<<ip<<" "<<port;
//        addDeviceSetWidget(new DeviceSetWidget(ip,port));
    }
}

void DeviceSetListWidget::addDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
    connect(deviceSetWidget,&DeviceSetWidget::commandSuccessed,
            [this]{
        if(_commandQueue.isEmpty())return ;
        if(--_counter!=0)return ;
        const proto::receiver::Command &successedCommand=_commandQueue.dequeue();

        if(successedCommand.command_type()==proto::receiver::START_DDC1){
            emit  ready(_deviceSetWidgetList);
        }else if(successedCommand.command_type()==proto::receiver::STOP_DDC1){
            emit notReady();
        }

        if(!_commandQueue.isEmpty()&&
                _commandQueue.head().command_type()!=successedCommand.command_type()){
            setAllDeviceSet(_commandQueue.head());
        }else setCursor(Qt::ArrowCursor);
    });

    QListWidgetItem *stationPanelItem = new QListWidgetItem(_listWidget);

    stationPanelItem->setSizeHint(deviceSetWidget->sizeHint());
    stationPanelItem->setSelected(true);
    _listWidget->setItemWidget(stationPanelItem,deviceSetWidget);
    DeviceSetWidget*dsw=
            _listWidget->findChild<DeviceSetWidget*>(deviceSetWidget->objectName());
    if(dsw){
        qDebug()<<"FIND CHILD";
    }else qDebug()<<"NOT FIND CHILD";
}

void DeviceSetListWidget::removeDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
    //TODO ИСПОЛЬЗОВАТЬ МЕТОДЫ ПОИСКА ВСТРИННЫЕ В QT - FIND (wid);
    DeviceSetWidget*dsw=
            _listWidget->findChild<DeviceSetWidget*>(deviceSetWidget->objectName());
    if(dsw){
        qDebug()<<"FIND CHILD";
    }else qDebug()<<"NOT FIND CHILD";

    for(int i=0;i<_listWidget->count();i++){
        QListWidgetItem *selectedItem=_listWidget->item(i);
        DeviceSetWidget *currentPanel=qobject_cast<DeviceSetWidget*>
                (_listWidget->itemWidget(selectedItem));
        if(currentPanel==deviceSetWidget){
            _listWidget->takeItem(_listWidget->row(selectedItem));
            _listWidget->removeItemWidget(selectedItem);
        }
    }
}

void DeviceSetListWidget::connectToSelectedDeviceSet()
{
    _deviceSetWidgetList.clear();

    QList<QListWidgetItem*> itemList=_listWidget->selectedItems();
    qDebug()<<"ConnectToSelection"<<itemList.size();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget){
            widget->connectToDeviceSet();
            _deviceSetWidgetList.append(widget);
        }else qDebug()<<"NULL DEVICE SET WIDGET";

    }
    setCursor(Qt::WaitCursor);

    checkingConnections(true);
}

void DeviceSetListWidget::disconnectFromSelectedDeviceSet()
{
    QList<QListWidgetItem*> itemList=_listWidget->selectedItems();
    qDebug()<<"ConnectToSelection"<<itemList.size();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(_listWidget->itemWidget(item));
        if(widget){
            widget->disconnectFromDeviceSet();
        }else qDebug()<<"NULL DEVICE SET WIDGET";

    }
    setCursor(Qt::WaitCursor);
    checkingConnections(false);
}

void DeviceSetListWidget::checkingConnections(bool checkingState)
{
    std::shared_ptr<QTimer> shpTimer=std::make_shared<QTimer>();
    std::shared_ptr<QTime> shpTime=std::make_shared<QTime>();
    std::shared_ptr<QMetaObject::Connection>shpConnection=
            std::make_shared<QMetaObject::Connection>();
    shpTime->start();

    *shpConnection=connect(shpTimer.get(),&QTimer::timeout,
                           [this,checkingState,shpTimer,shpTime,shpConnection]{
        bool success=false;
        for (DeviceSetWidget*deviceSetWidget:_deviceSetWidgetList)
            if(checkingState==deviceSetWidget->isConnected()){
                success=true;
            }else {
                success=false;
                break;
            }

        if(!success&&shpTime->elapsed()>=TIME_WAIT_CONNECTION){
            QObject::disconnect(*shpConnection);
            setCursor(Qt::ArrowCursor);
            QMessageBox::warning(this,"Connection Failed","");
        }else if(success){
            QObject::disconnect(*shpConnection);
            setCursor(Qt::ArrowCursor);
        }
    });
    shpTimer->start(500);
}

void DeviceSetListWidget::setCursor(const QCursor &cursor)
{
    //TODO НЕПОНЯТНО ПОЧЕМУ ДВА PARENT WIDGET НУЖНО СТАВИТЬ
    if(parentWidget())
        parentWidget()->parentWidget()->setCursor(cursor);
}

void DeviceSetListWidget::onRemoveDeviceSetWidget()
{
    QList<QListWidgetItem*> selectedItems=_listWidget->selectedItems();
    for(QListWidgetItem *selectedItem:selectedItems){
        _listWidget->removeItemWidget(selectedItem);
        delete _listWidget->takeItem(_listWidget->row(selectedItem));
    }
}

void DeviceSetListWidget::onStationItemSelected(QListWidgetItem *item)
{
    qDebug()<<"Station Panel Clicked";
    if(_listWidget->selectedItems().isEmpty()){
        _pbConnectToStation->setEnabled(false);
    }else {
        _pbConnectToStation->setEnabled(true);
    }
}

void DeviceSetListWidget::onAddDeviceSetWidget()
{
    bool ok;
    QString text= QInputDialog::getText(this,"Creation Device Set Client",
                                        "Ip address:port",
                                        QLineEdit::Normal,
                                        "address:port",&ok);
    QStringList list=text.split(":");


    QString address=list[0];
    quint16 port =list[1].toUShort();

    if(ok&&port!=0){
        this->addDeviceSetWidget(new DeviceSetWidget(address,port));
    }else   QMessageBox::
            warning(this,"Creation Device Set Client faild","address:port");

}
