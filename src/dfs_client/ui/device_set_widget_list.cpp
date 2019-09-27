#include "device_set_widget_list.h"

#include "device_set_widget.h"

#include <QDebug>

DeviceSetListWidget::DeviceSetListWidget(QWidget *parent)
    :QListWidget (parent)
{
    setSelectionMode(QAbstractItemView::MultiSelection);

    connect(this,&QListWidget::itemClicked,
            this,&DeviceSetListWidget::onStationItemSelected);
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
    QList<QListWidgetItem*> itemList=selectedItems();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(itemWidget(item));
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
    QListWidgetItem *stationPanelItem = new QListWidgetItem(this);
    stationPanelItem->setSizeHint(deviceSetWidget->sizeHint());
    stationPanelItem->setSelected(true);
    setItemWidget(stationPanelItem,deviceSetWidget);
}

void DeviceSetListWidget::removeDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
    //TODO ИСПОЛЬЗОВАТЬ МЕТОДЫ ПОИСКА ВСТРИННЫЕ В QT - FIND (wid);
    for(int i=0;i<count();i++){
        QListWidgetItem *selectedItem=item(i);
        DeviceSetWidget *currentPanel=qobject_cast<DeviceSetWidget*>
                (itemWidget(selectedItem));
        if(currentPanel==deviceSetWidget){
            takeItem(row(selectedItem));
            removeItemWidget(selectedItem);
        }
    }
}

void DeviceSetListWidget::onStationItemSelected(QListWidgetItem *item)
{
    qDebug()<<"Station Panel Clicked";
    //    DeviceSetListWidget*panel=
    //            qobject_cast<DeviceSetWidget*>(itemWidget(item));
    //    if(panel) emit receiverStationPanelSelected(panel,item->isSelected());
}

void DeviceSetListWidget::connectToSelectedDeviceSet()
{
    QList<QListWidgetItem*> itemList=selectedItems();
    qDebug()<<"ConnectToSelection"<<itemList.size();
    for(QListWidgetItem*item:itemList){
        DeviceSetWidget*widget=qobject_cast<DeviceSetWidget*>(itemWidget(item));
        if(widget){
            widget->connectToDeviceSet();
        }else {
            qDebug()<<"NULL DEVICE SET WIDGET";
        }
    }
}
