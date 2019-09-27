#include "device_set_widget_list.h"

#include "device_set_widget.h"

#include <QDebug>

DeviceSetListWidget::DeviceSetListWidget()
{
    setSelectionMode(QAbstractItemView::MultiSelection);

    connect(this,&QListWidget::itemClicked,
            this,&DeviceSetListWidget::onStationItemSelected);
}

void DeviceSetListWidget::addDeviceSetWidget(DeviceSetWidget *deviceSetWidget)
{
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
