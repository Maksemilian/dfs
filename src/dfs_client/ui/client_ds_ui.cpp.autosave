#include "client_ds_ui.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>

const QString DeviceWidget::STRING_CONNECT = "connected";
const QString DeviceWidget::STRING_DISCONNECT = "disconnect";

DeviceWidget::DeviceWidget(const QString& name):
    _lbName(new QLabel(name, this)),
    _lbDeviceActivatedStatus(new QLabel(this)),
    _lbStreamDDC1StartedStatus(new QLabel(STRING_CONNECT, this)),
    _cbReceivers(new QComboBox(this))
{
    setObjectName(name);

    QGridLayout* vbLayout = new QGridLayout;
    setLayout(vbLayout);

    vbLayout->addWidget(new QLabel("Address:", this), 0, 0);
    vbLayout->addWidget(_lbName, 0, 1);

    vbLayout->addWidget(new QLabel("Receivers:"), 2, 0);
    vbLayout->addWidget(_cbReceivers, 2, 1);

    vbLayout->addWidget(new QLabel("Status:"), 3, 0);
    vbLayout->addWidget(_lbDeviceActivatedStatus);

    vbLayout->addWidget(new QLabel("DDC1:"), 4, 0);
    vbLayout->addWidget(_lbStreamDDC1StartedStatus, 4, 1);

    vbLayout->addWidget(new QLabel("DDC1:"), 4, 0);
    vbLayout->addWidget(_lbStreamDDC1StartedStatus, 4, 1);

    _lbDeviceActivatedStatus->setText(STRING_DISCONNECT);
}

void DeviceWidget::setName(const QString& address)
{
    _lbName->setText(address);
}

QString DeviceWidget::getName()
{
    return _lbName->text();
}

void DeviceWidget::onDeviceOpen(const QStringList& receivers)
{
    qDebug() << "DEVICE OPENED";
    _lbDeviceActivatedStatus->setText(STRING_CONNECT);
    _cbReceivers->addItems(receivers);
}

void DeviceWidget::onDeviceClose()
{
    qDebug() << "DEVICE CLOSED";
    _lbDeviceActivatedStatus->setText(STRING_DISCONNECT);
    _lbDeviceActivatedStatus->setUserData(0, nullptr);
    _cbReceivers->clear();
}

void DeviceWidget::onShowDeviceError(const QString& errorString)
{
    _lbDeviceActivatedStatus->setText(errorString);
}


//************** DELETE **********
/*
class PhaseValidator: public QValidator
{
  public:
    State validate(QString& str, int& pos)const override
    {
        Q_UNUSED(pos);
        if(str.contains(QRegExp("[0-9]")))
        {
            double val = str.toDouble();
            if(val >= -180 && val <= 180)
            {
                return  Acceptable;
            }
            else
            {
                return Intermediate;
            }
        }
        else
        {
            qDebug() << "ELSE REG";
        }
        if(str.isEmpty())
        {
            return Intermediate;
        }
        return Invalid;
    }
};
*/
//    proto::receiver::Command command;
//    command.set_command_type(proto::receiver::SET_DEVICE_INDEX);
//    command.set_device_set_index(_lbDeviceSetIndex->text().toUInt());
//    command.set_demo_mode(true);
//    sendCommand(command);

//   connect(_lbDeviceSetIndex, static_cast<void(QComboBox::*)(int)>(&QComboBox::activated),
//            [this](int index)
//    {
//        proto::receiver::Command command;
//        command.set_command_type(proto::receiver::SET_DEVICE_INDEX);
//        command.set_device_set_index(static_cast<quint32>(index));
//        this->sendCommand(command);
//    });

//    connect(_leSetShiftPhaseDDC1, &QLineEdit::editingFinished,
//            [this]()
//    {
//        double phaseShiftDDC1 = _leSetShiftPhaseDDC1->text().toDouble();
//        qDebug() << phaseShiftDDC1;
//        proto::receiver::Command command;
//        command.set_command_type(proto::receiver::CommandType::SET_SHIFT_PHASE_DDC);
//        proto::receiver::ShiftPhaseDDC1* shiftPhaseDDC1 =
//            new proto::receiver::ShiftPhaseDDC1();
//        shiftPhaseDDC1->set_device_index(static_cast<quint32>(
//                                             _cbReceivers->currentIndex()));
//        shiftPhaseDDC1->set_phase_shift(phaseShiftDDC1);
//        command.set_allocated_shift_phase_ddc1(shiftPhaseDDC1);
//        qDebug() << "WARNING NOT USE";
////        this->sendCommand(command);
//    });

/*
bool DeviceSetWidget::isConnected()
{
    if(QObjectUserData* userData = _lbStatus->userData(USER_DATA_STATUS))
    {
        Status* status = dynamic_cast<Status*>(userData);
        return status->_status;
    }
    return false;
}

void DeviceSetWidget::setListeningStreamPort(quint16 port)
{
    _deviceSetClient->setLiceningStreamPort(port);
}

void DeviceSetWidget::setStatus(bool status)
{
    if(QObjectUserData* userData = _lbStatus->userData(USER_DATA_STATUS))
    {
        Status* s = dynamic_cast<Status*>(userData);
        s->_status = status;
    }
}
void DeviceSetWidget::sendCommand(proto::receiver::Command& command)
{
    _deviceSetClient->sendCommand(command);
}

*/
