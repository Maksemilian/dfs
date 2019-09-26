#ifndef RECEIVER_STATION_DATA_WIDGET_H
#define RECEIVER_STATION_DATA_WIDGET_H

#include <QWidget>
#include <QListWidgetItem>
#include <QDebug>

namespace Ui {
class ReceiverStationPanel;
}

class ReceiverStationPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiverStationPanel(QWidget *parent = nullptr);
    ~ReceiverStationPanel(){qDebug()<<"DESTR RECEIVER STATION PANEL";}
    void setAddress(const QString &strStationAddress,const QString &strStationPort);
    void addReceiver(const QString &receiverName);
    QString getCurrentDeviceSetName();
    QString getIpAddress();

    int currentReceiverIndex();
private:
    Ui::ReceiverStationPanel *ui=nullptr;
};

#endif // RECEIVER_STATION_DATA_WIDGET_H
