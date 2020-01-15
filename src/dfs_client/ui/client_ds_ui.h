#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include <QWidget>

class QComboBox;
class QLabel;
class QLineEdit;

class DeviceSetWidget : public QWidget
{
    Q_OBJECT
    static const QString STRING_CONNECT;
    static const QString STRING_DISCONNECT;
    static const QString TEXT_CONNECT;
    static const QString TEXT_DISCONNECT;
    static const int USER_DATA_STATUS = 0;

  public:
    DeviceSetWidget(const QString& address, quint16 port);
    void setAddress(const QString& address, quint16 port);
    QString address();
    quint16 port();

  signals:
    void getDeviceInfoUpdate(quint32 numberDeviceSet);
  public slots:
    void onDeviceSetReady();
    void onDeviceSetInfoUpdate(const QStringList& receivers);
    void onDeviceSetDisconnected();
    void onDeviceSetCommandFailed(const QString& errorString);
  private:
    void setCursor(const QCursor& cursor);
  private:
    QLabel* _lbAddresText;
    QLabel* _lbPort;
    QLabel* _lbStatus;
    QLabel* _lbStatusDDC1;
    QComboBox* _cbReceivers;
    QLineEdit* _leSetShiftPhaseDDC1;
    QLabel* _lbDeviceSetIndex;
};

#endif // DEVICE_SET_WIDGET_H
