#include "receiver_station_panel.h"
#include "ui_receiver_station_panel.h"


ReceiverStationPanel::ReceiverStationPanel(QWidget *parent):
    QWidget(parent),ui(new Ui::ReceiverStationPanel){
    ui->setupUi(this);

    ui->cbReceivers->setEditable(false);
//    ui->cbReceivers->setCurrentIndex(0);

}

QString ReceiverStationPanel::getCurrentDeviceSetName()
{
    QString deveiceSetName="DS#"+ui->cbReceivers->itemText(0);
    for(int i=1;i<ui->cbReceivers->count();i++){
       deveiceSetName+="_"+ui->cbReceivers->itemText(i);
    }
    return deveiceSetName;
}

QString ReceiverStationPanel::getIpAddress()
{
    return ui->lbStaionIp->text();
}

void ReceiverStationPanel::setAddress(const QString &strStationAddress,const QString &strStationPort)
{
    ui->lbStaionIp->setText(strStationAddress);
    ui->lbStationPort->setText(strStationPort);
}

void ReceiverStationPanel::addReceiver(const QString &receiverName)
{
    ui->cbReceivers->addItem(receiverName);
}

int ReceiverStationPanel::currentReceiverIndex()
{
    return ui->cbReceivers->currentIndex();
}
