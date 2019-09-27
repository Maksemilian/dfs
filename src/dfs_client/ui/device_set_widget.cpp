#include "device_set_widget.h"

#include "core/network/device_set_client.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QHostAddress>

DeviceSetWidget::DeviceSetWidget(const QString &address,quint16 port):
    _lbAddresText(new QLabel(address,this)),
    _lbPort(new QLabel(QString::number(port),this)),
    _cbReceivers(new QComboBox(this)),
    _deviceSetClient(std::make_unique<DeviceSetClient>())
{
    QGridLayout *vbLayout=new QGridLayout;
    setLayout(vbLayout);

    vbLayout->addWidget(new QLabel("Address:",this),0,0);
    vbLayout->addWidget(_lbAddresText,0,1);

    vbLayout->addWidget(new QLabel("Port:",this),1,0);
    vbLayout->addWidget(_lbPort,1,1);

    vbLayout->addWidget(_cbReceivers);

    connect(_deviceSetClient.get(),&DeviceSetClient::deviceSetReady,
            this,&DeviceSetWidget::onDeviceSetReady);

    connect(_deviceSetClient.get(),&DeviceSetClient::commandSuccessed,
            this,&DeviceSetWidget::commandSuccessed);

    connect(_deviceSetClient.get(),&DeviceSetClient::disconnected,
            this,&DeviceSetWidget::onDeviceSetDisconnected);

    connect(_deviceSetClient.get(),&DeviceSetClient::commandFailed,
            this,&DeviceSetWidget::onDeviceSetCommandFailed);
}

void DeviceSetWidget::connectToDeviceSet()
{
    _deviceSetClient->connectToHost(QHostAddress(_lbAddresText->text()),
                                    _lbPort->text().toUShort());
}

void DeviceSetWidget::onDeviceSetReady()
{
    qDebug()<<"onDeviceSetReady";
   _cbReceivers->addItems(_deviceSetClient->receiverNameList());
}

void DeviceSetWidget::onDeviceSetDisconnected()
{
    qDebug()<<"DeviceSet disconnected";
//TODO СДЕЛАТЬ ВИЗУАЛИЗАЦИЮ ДЛЯ ОТКЛЮЧЕНИЯ
}

void DeviceSetWidget::onDeviceSetCommandFailed(const QString &errorString)
{
qDebug()<<errorString;
//TODO СДЕЛАТЬ ВИЗУАЛИЗАЦИЮ ДЛЯ ОШИБКИ
}

void DeviceSetWidget::setDeviceSetCommand(const proto::receiver::Command &command)
{
    _deviceSetClient->sendCommand(command);
}

void DeviceSetWidget::setAddres(const QString &address, quint16 port)
{
    _lbAddresText->setText(address);
    _lbPort->setText(QString::number(port));
}

QString DeviceSetWidget::address()
{
    return _lbAddresText->text();
}

quint16 DeviceSetWidget::port()
{
    return  _lbPort->text().toUShort();
}

