#include "client_device_set_widget.h"
#include "client_device_set.h"
#include "client_stream_controller.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QHostAddress>
#include <QTimer>
#include <QMessageBox>

const QString DeviceSetWidget::STRING_CONNECT="connected";
const QString DeviceSetWidget::STRING_DISCONNECT="disconnect";

const QString DeviceSetWidget::TEXT_CONNECT="connect";
const QString DeviceSetWidget::TEXT_DISCONNECT="disconnect";

DeviceSetWidget::DeviceSetWidget(const QString &address,quint16 port):
    _lbAddresText(new QLabel(address,this)),
    _lbPort(new QLabel(QString::number(port),this)),
    _lbStatus(new QLabel(this)),
    _lbStatusDDC1(new QLabel(TEXT_DISCONNECT,this)),
    _cbReceivers(new QComboBox(this)),
    _deviceSetClient(std::make_unique<DeviceSetClient>()),
    _streamController(std::make_unique<ClientStreamController>(address,port))
{
    setObjectName(address);
    QGridLayout *vbLayout=new QGridLayout;
    setLayout(vbLayout);

    vbLayout->addWidget(new QLabel("Address:",this),0,0);
    vbLayout->addWidget(_lbAddresText,0,1);

    vbLayout->addWidget(new QLabel("Port:",this),1,0);
    vbLayout->addWidget(_lbPort,1,1);

    vbLayout->addWidget(new QLabel("Receivers:"),2,0);
    vbLayout->addWidget(_cbReceivers,2,1);

    vbLayout->addWidget(new QLabel("Status:"),3,0);
    vbLayout->addWidget(_lbStatus);

    vbLayout->addWidget(new QLabel("DDC1"),4,0);
    vbLayout->addWidget(_lbStatusDDC1,4,1);

    _lbStatus->setText(STRING_DISCONNECT);
    _lbStatus->setUserData(USER_DATA_STATUS,new Status(false));

    connect(_streamController.get(),&ClientStreamController::ddcStarted,
            [this]{
        _lbStatusDDC1->setText(TEXT_CONNECT);
    });

    connect(_streamController.get(),&ClientStreamController::ddcStoped,
            [this]{
        _lbStatusDDC1->setText(TEXT_DISCONNECT);
    });

    connect(_deviceSetClient.get(),&DeviceSetClient::deviceSetReady,
            this,&DeviceSetWidget::onDeviceSetReady);

    connect(_deviceSetClient.get(),&DeviceSetClient::commandSuccessed,
            this,&DeviceSetWidget::commandSuccessed);

    connect(_deviceSetClient.get(),&DeviceSetClient::disconnected,
            this,&DeviceSetWidget::onDeviceSetDisconnected);

    connect(_deviceSetClient.get(),&DeviceSetClient::commandFailed,
            this,&DeviceSetWidget::onDeviceSetCommandFailed);

    connect(_deviceSetClient.get(),&DeviceSetClient::ddc1StreamStarted,
            this,&DeviceSetWidget::onDDC1Started);

    connect(_deviceSetClient.get(),&DeviceSetClient::ddc1StreamStoped,
            this,&DeviceSetWidget::onDDC1Stoped);
}

void DeviceSetWidget::connectToDeviceSet()
{
    _deviceSetClient->connectToHost(QHostAddress(_lbAddresText->text()),
                                    _lbPort->text().toUShort());
}

void DeviceSetWidget::disconnectFromDeviceSet()
{
    _deviceSetClient->disconnectFromHost();
}

void DeviceSetWidget::onDDC1Started()
{
    _streamController->startDDC();
}

void DeviceSetWidget::onDDC1Stoped()
{
    _streamController->stopDDC();
}

void DeviceSetWidget::onDeviceSetReady()
{
    qDebug()<<"onDeviceSetReady";
    _lbStatus->setText(STRING_CONNECT);
    setStatus(true);
    _cbReceivers->addItems(_deviceSetClient->receiverNameList());
}

void DeviceSetWidget::onDeviceSetDisconnected()
{
    qDebug()<<"DeviceSet disconnected";
    _lbStatus->setText(STRING_DISCONNECT);
    setStatus(false);
    _cbReceivers->clear();
    //TODO СДЕЛАТЬ ВИЗУАЛИЗАЦИЮ ДЛЯ ОТКЛЮЧЕНИЯ
}

bool DeviceSetWidget::isConnected()
{
    if(QObjectUserData *userData=_lbStatus->userData(USER_DATA_STATUS))
    {
        Status*status=dynamic_cast<Status*>(userData);
        return status->_status;
    }
    return false;
}


void DeviceSetWidget::setStatus(bool status)
{
    if(QObjectUserData *userData=_lbStatus->userData(USER_DATA_STATUS))
    {
        Status*s=dynamic_cast<Status*>(userData);
        s->_status=status;
    }
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

