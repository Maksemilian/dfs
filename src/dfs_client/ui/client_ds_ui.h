#ifndef DEVICE_SET_WIDGET_H
#define DEVICE_SET_WIDGET_H

#include <memory>

#include <QWidget>
#include <QLabel>

class QComboBox;

class AspectRatioLabel : public QLabel
{
    Q_OBJECT
  public:
    explicit AspectRatioLabel(QWidget* parent = 0);
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;
  public slots:
    void setPixmap ( const QPixmap& );
    void resizeEvent(QResizeEvent*);
  private:
    QPixmap pix;
};

class DeviceWidget : public QWidget
{
    Q_OBJECT
    static const QString CONNECT_IMG;
    static const QString DISCONNECT_IMG;
    static const QString ERROR_IMG;
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
    AspectRatioLabel* _status;
    QComboBox* _cbReceivers;
};

#endif // DEVICE_SET_WIDGET_H
