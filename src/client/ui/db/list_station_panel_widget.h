#ifndef LIST_STATION_PANEL_WIDGET_H
#define LIST_STATION_PANEL_WIDGET_H

#include <QListWidget>

class ReceiverStationPanel;

class ListStationPanelWidget:public QListWidget
{
    Q_OBJECT
public:
    ListStationPanelWidget( QWidget*parent=nullptr);
    void appendReceiverStationPanel(ReceiverStationPanel*panel);
    void removeReceiverStationPanel(ReceiverStationPanel*panel);
    QStringList getDeviceSetNameList();
    QList<ReceiverStationPanel*>getPanels();
Q_SIGNALS:
    void receiverStationPanelSelected(ReceiverStationPanel*panel,bool selecState);
private Q_SLOTS:
    void onStationItemSelected(QListWidgetItem *item);
};

#endif // LIST_STATION_PANEL_WIDGET_H
