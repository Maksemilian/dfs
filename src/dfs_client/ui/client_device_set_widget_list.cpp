#include "device_set_widget_list.h"

#include "device_set_widget.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>

DeviceSetListWidget::DeviceSetListWidget(QWidget *parent)
//    :QListWidget (parent)
    :QWidget(parent),_deviceSetListWidget(new QListWidget(this))
{
    setLayout(new QVBoxLayout);
    _deviceSetListWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    _pbAddDeviceSetWidget=new QPushButton("Add",this);
    _pbRemoveDeviceSetWidget=new QPushButton("Remove",this);

    layout()->addWidget(_deviceSetListWidget);
    layout()->addWidget(_pbAddDeviceSetWidget);
    layout()->addWidget(_pbRemoveDeviceSetWidget);


    connect(_deviceSetListWidget,&QListWidget::itemClicked,
            this,&DeviceSetListWidget::onStationItemSelected);

    connect(_pbAddDeviceSetWidget,&QPushButton::clicked,[this]{
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
            if(_deviceSetListWidget->selectedItems().isEmpty()){
                _pbRemoveDeviceSetWidget->setEnabled(false);
            }else {
                _pbRemoveDeviceSetWidget->setEnabled(true);
            }
        }else {
            QMessageBox::warning(this,"Creation Device Set Client faild","address:port");
        }
    });

    connect(_pbRemoveDeviceSetWidget,&QPushButton::clicked,
            this,&DeviceSetListWidget::removeSelected);
}

void DeviceSetListWidget::setCommand(const proto::receiver::Command &command)
{
    //TODO НЕПОНЯТНО ПОЧЕМУ ДВА PARENT WIDGET НУЖНО СТАВИТЬ
    if(parentWidget()->parentWidget())
        parentWidget()->parentWidget()->setCursor(Qt::WaitCursor);

    if(_counter>0){
        _commandQueue.enqueue(command);
        return;
    }
    //TODO ВАРИАНТ НИЖЕ ДАЕТ КРАХ ПРОГРАММЫ
    //    if(_commandQueue.isEmpty()){
    //        _commandQueue.enqueue(command);
    //    }else {
    //        _commandQueue.enqueue(command);
    //        return;
    //    }

    //            command.command_type()!=_commandQueue.head().command_type())
    setAllDeviceSet(command);
}

void DeviceSetListWidget::setAllDeviceSet(const proto::receiver::Command &command)
{
    QList<QListWidgetItem*> itemList=_deviceSetListWidget->selectedItems();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(_deviceSetListWidget->itemWidget(item));
        if(widget){
            widget->setDeviceSetCommand(command);
            _counter++;
        }
    }
}

void DeviceSetListWidget::addDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
    connect(deviceSetWidget,&DeviceSetWidget::commandSuccessed,
            [this]{
        if((--_counter)==0){
            qDebug()<<"ALL COMMAND SUCCESSED";
            if(parentWidget()->parentWidget())
                parentWidget()->parentWidget()->setCursor(Qt::ArrowCursor);

            if(!_commandQueue.isEmpty())
                setAllDeviceSet(_commandQueue.dequeue());
        }
        //        qDebug()<<"Here_1";
        //        const proto::receiver::Command &successedCommand=_commandQueue.dequeue();
        //        qDebug()<<"Here_2";
        //        if(!_commandQueue.isEmpty()&&
        //                _commandQueue.head().command_type()
        //                !=successedCommand.command_type()){
        //            setAllDeviceSet(_commandQueue.head());
        //        }else if(parentWidget()->parentWidget()){
        //            parentWidget()->parentWidget()->setCursor(Qt::ArrowCursor);
        //        }
    });
    QListWidgetItem *stationPanelItem = new QListWidgetItem(_deviceSetListWidget);
    stationPanelItem->setSizeHint(deviceSetWidget->sizeHint());
    stationPanelItem->setSelected(true);
    _deviceSetListWidget->setItemWidget(stationPanelItem,deviceSetWidget);
}

void DeviceSetListWidget::removeDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
    //TODO ИСПОЛЬЗОВАТЬ МЕТОДЫ ПОИСКА ВСТРИННЫЕ В QT - FIND (wid);
    for(int i=0;i<_deviceSetListWidget->count();i++){
        QListWidgetItem *selectedItem=_deviceSetListWidget->item(i);
        DeviceSetWidget *currentPanel=qobject_cast<DeviceSetWidget*>
                (_deviceSetListWidget->itemWidget(selectedItem));
        if(currentPanel==deviceSetWidget){
            _deviceSetListWidget->takeItem(_deviceSetListWidget->row(selectedItem));
            _deviceSetListWidget->removeItemWidget(selectedItem);
        }
    }
}

void DeviceSetListWidget::removeSelected()
{
    QList<QListWidgetItem*> selectedItems=_deviceSetListWidget->selectedItems();
    for(QListWidgetItem *selectedItem:selectedItems){
        _deviceSetListWidget->removeItemWidget(selectedItem);
        delete _deviceSetListWidget->takeItem(_deviceSetListWidget->row(selectedItem));
    }
    if(_deviceSetListWidget->selectedItems().isEmpty())
        _pbRemoveDeviceSetWidget->setEnabled(false);
}

void DeviceSetListWidget::onStationItemSelected(QListWidgetItem *item)
{
    qDebug()<<"Station Panel Clicked";
    if(_deviceSetListWidget->selectedItems().isEmpty()){
        _pbRemoveDeviceSetWidget->setEnabled(false);
    }else {
        _pbRemoveDeviceSetWidget->setEnabled(true);
    }
    //    DeviceSetListWidget*panel=
    //            qobject_cast<DeviceSetWidget*>(itemWidget(item));
    //    if(panel) emit receiverStationPanelSelected(panel,item->isSelected());
}

void DeviceSetListWidget::connectToSelectedDeviceSet()
{
    QList<QListWidgetItem*> itemList=_deviceSetListWidget->selectedItems();
    qDebug()<<"ConnectToSelection"<<itemList.size();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(_deviceSetListWidget->itemWidget(item));
        if(widget){
            widget->connectToDeviceSet();
        }else {
            qDebug()<<"NULL DEVICE SET WIDGET";
        }
    }
}
