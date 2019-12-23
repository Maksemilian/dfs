#include "client_ds_ui.h"
#include "client_ds.h"
#include "client_ds_stream_controller.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QHostAddress>
#include <QTimer>
#include <QMessageBox>
#include <QLineEdit>

const QString DeviceSetWidget::STRING_CONNECT="connected";
const QString DeviceSetWidget::STRING_DISCONNECT="disconnect";

const QString DeviceSetWidget::TEXT_CONNECT="connect";
const QString DeviceSetWidget::TEXT_DISCONNECT="disconnect";

class PhaseValidator:public QValidator
{
public:
    State validate(QString &str,int &pos)const override
    {
        if(str.contains(QRegExp("[0-9]"))){
            double val=str.toDouble();
            if(val>=-180&&val<=180)
                return  Acceptable;
            else {
                return Intermediate;
            }
        }else {
            qDebug()<<"ELSE REG";
        }
        if(str.isEmpty())    return Intermediate;
        return Invalid;
    }
};

DeviceSetWidget::DeviceSetWidget(const QString &address,quint16 port):
    _lbAddresText(new QLabel(address,this)),
    _lbPort(new QLabel(QString::number(port),this)),
    _lbStatus(new QLabel(this)),
    _lbStatusDDC1(new QLabel(TEXT_DISCONNECT,this)),
    _cbReceivers(new QComboBox(this)),
    _leSetShiftPhaseDDC1(new QLineEdit(this)),
    _cbDeviceSetIndex(new QComboBox(this)),
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

    vbLayout->addWidget(new QLabel("DDC1:"),4,0);
    vbLayout->addWidget(_lbStatusDDC1,4,1);

    vbLayout->addWidget(new QLabel("Phase Shift ddc1:"),5,0);
    vbLayout->addWidget(_leSetShiftPhaseDDC1);
    _leSetShiftPhaseDDC1->setValidator(new PhaseValidator);
//    _leSetShiftPhaseDDC1->setInputMask("DDD");
    vbLayout->addWidget(new QLabel("DS index:"),6,0);
    vbLayout->addWidget(_cbDeviceSetIndex,6,1);
    _cbDeviceSetIndex->addItem("0");
    _cbDeviceSetIndex->addItem("1");

    connect(_cbDeviceSetIndex,static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
            [this](int index){
        proto::receiver::Command command;
        command.set_command_type(proto::receiver::SET_DEVICE_INDEX);
        command.set_device_set_index(static_cast<quint32>(index));
        this->sendCommand(command);
    });

    connect(_leSetShiftPhaseDDC1,&QLineEdit::editingFinished,
            [this](){
        double phaseShiftDDC1=_leSetShiftPhaseDDC1->text().toDouble();
      qDebug()<<phaseShiftDDC1;
      proto::receiver::Command command;
      command.set_command_type(proto::receiver::CommandType::SET_SHIFT_PHASE_DDC);
      proto::receiver::ShiftPhaseDDC1 *shiftPhaseDDC1=
              new proto::receiver::ShiftPhaseDDC1();
      shiftPhaseDDC1->set_device_index(static_cast<quint32>(
                                           _cbReceivers->currentIndex()));
      shiftPhaseDDC1->set_phase_shift(phaseShiftDDC1);
      command.set_allocated_shift_phase_ddc1(shiftPhaseDDC1);
      this->sendCommand(command);
    });

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

    connect(_deviceSetClient.get(),&DeviceSetClient::connected,
            this,&DeviceSetWidget::onDeviceSetConnected);

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

void DeviceSetWidget::onDeviceSetConnected()
{
    proto::receiver::Command command;
    command.set_command_type(proto::receiver::SET_DEVICE_INDEX);
    command.set_device_set_index(static_cast<quint32>(_cbDeviceSetIndex->currentIndex()));
    sendCommand(command);
}

void DeviceSetWidget::onDeviceSetDisconnected()
{
    qDebug()<<"DeviceSet disconnected";
    _lbStatus->setText(STRING_DISCONNECT);
    setStatus(false);
    _cbReceivers->clear();
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
}

void DeviceSetWidget::sendCommand(const proto::receiver::Command &command)
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

