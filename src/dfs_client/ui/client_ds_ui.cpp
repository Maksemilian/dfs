#include "client_ds_ui.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QDebug>

AspectRatioLabel::AspectRatioLabel(QWidget* parent) :
    QLabel(parent)
{
    this->setMinimumSize(1, 1);
    setScaledContents(false);
}

void AspectRatioLabel::setPixmap ( const QPixmap& p)
{
    pix = p;
    QLabel::setPixmap(scaledPixmap());
}

int AspectRatioLabel::heightForWidth( int width ) const
{
    return pix.isNull() ?
           this->height() :
           (pix.height() * width) / pix.width();
}

QSize AspectRatioLabel::sizeHint() const
{
    return  QSize(20, 20);
}

QPixmap AspectRatioLabel::scaledPixmap() const
{
    return pix.scaled(this->size(),
                      Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void AspectRatioLabel::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
}
///! \addtogroup client

///@{

const QString DeviceWidget::CONNECT_IMG = ":images/green_circle.png";
const QString DeviceWidget::DISCONNECT_IMG = ":images/grey_circle.png";
const QString DeviceWidget::ERROR_IMG = ":images/red_circle.png";

DeviceWidget::DeviceWidget(const QString& name):
    _lbName(new QLabel(name, this)),
    _status(new AspectRatioLabel(this)),
    _cbReceivers(new QComboBox(this))
{
    setObjectName(name);
    _status->setPixmap(QPixmap(DISCONNECT_IMG));

    QGridLayout* vbLayout = new QGridLayout;
    setLayout(vbLayout);
    vbLayout->addWidget(_status, 0, 0);

    vbLayout->addWidget(new QLabel("Name:", this), 0, 1);
    vbLayout->addWidget(_lbName, 0, 2);

    vbLayout->addWidget(new QLabel("Receivers:"), 1, 1);
    vbLayout->addWidget(_cbReceivers, 1, 2);
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
    _status->setPixmap(QPixmap(CONNECT_IMG));
    _cbReceivers->addItems(receivers);
}

void DeviceWidget::onDeviceClose()
{
    qDebug() << "DEVICE CLOSED";
    _status->setPixmap(QPixmap(DISCONNECT_IMG));
    _cbReceivers->clear();
}

void DeviceWidget::onShowDeviceError(const QString& errorString)
{
    _status->setPixmap(QPixmap(ERROR_IMG));
}

///@}
