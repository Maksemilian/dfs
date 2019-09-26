#include "list_station_panel_widget.h"

#include "receiver_station_panel.h"

#include <QDebug>

ListStationPanelWidget::ListStationPanelWidget(QWidget*parent)
    :QListWidget (parent)
{
    setSelectionMode(QAbstractItemView::MultiSelection);
    connect(this,&QListWidget::itemClicked,
            this,&ListStationPanelWidget::onStationItemSelected);
}

void ListStationPanelWidget::onStationItemSelected(QListWidgetItem *item)
{
    qDebug()<<"Station Panel Clicked";
    ReceiverStationPanel*panel=
            qobject_cast<ReceiverStationPanel*>(itemWidget(item));
    if(panel) emit receiverStationPanelSelected(panel,item->isSelected());
}

void ListStationPanelWidget::appendReceiverStationPanel(ReceiverStationPanel *panel)
{
    QListWidgetItem *stationPanelItem = new QListWidgetItem(this);
    stationPanelItem->setSizeHint(panel->sizeHint());
    stationPanelItem->setSelected(true);
    setItemWidget(stationPanelItem,panel);
    //WARNING ЕСЛИ ОСТАВИТЬ НИЖНЮЮ СТРОЧКУ АЙТЕМЫ СТАНОВЯТСЯ НЕ КЛИКАБЕЛЬНЫМИ
//    panel->setParent(this);
}

void ListStationPanelWidget::removeReceiverStationPanel(ReceiverStationPanel *panel)
{
    for(int i=0;i<count();i++){
        QListWidgetItem *selectedItem=item(i);
        ReceiverStationPanel *currentPanel=qobject_cast<ReceiverStationPanel*>
                (itemWidget(selectedItem));
        if(currentPanel==panel){
            takeItem(row(selectedItem));
            removeItemWidget(selectedItem);
        }
    }
}

QList<ReceiverStationPanel*>ListStationPanelWidget::getPanels()
{
    QList<ReceiverStationPanel*>panels;
    for(int i=0;i<count();i++){
        QListWidgetItem *selectedItem=item(i);
        ReceiverStationPanel *currentPanel=qobject_cast<ReceiverStationPanel*>
                (itemWidget(selectedItem));
        if(currentPanel)
            panels<<currentPanel;
    }
    return  panels;
}

QStringList ListStationPanelWidget::getDeviceSetNameList()
{
    QList<QListWidgetItem* >list=selectedItems();

    QStringList deviceSetNameList;
    for(QListWidgetItem *item:list){
        ReceiverStationPanel*panel= qobject_cast<ReceiverStationPanel*>(itemWidget(item));
        deviceSetNameList<<panel->getCurrentDeviceSetName();
    }
    return deviceSetNameList;
}
