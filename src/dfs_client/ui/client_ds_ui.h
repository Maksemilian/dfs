#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include <QWidget>

class QComboBox;
class QLabel;

class DeviceWidget : public QWidget
{
    Q_OBJECT
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;

  public:
    DeviceWidget(const QString& name);
    void setName(const QString& name);
    QString getName();

  public slots:
    void onDeviceOpen(const QStringList& receivers);
    void onDeviceClose();
    void onShowDeviceError(const QString& errorString);
  private:
  private:
    QLabel* _lbName;
    QLabel* _lbDeviceActivatedStatus;
    QLabel* _lbStreamDDC1StartedStatus;
    QComboBox* _cbReceivers;
};

#endif // DEVICE_SET_WIDGET_H
