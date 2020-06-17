#ifndef PRESELECTOR_WIDGET_H
#define PRESELECTOR_WIDGET_H


#include <QWidget>
class QComboBox;

/*! \addtogroup client
 */

///@{

using  UInt32LowFreq = quint32 ;
using  UInt32HeighFreq = quint32 ;
/*!
 * \brief Виджет для ввода данных преселектора.
 * \todo PreselectorWidget сделать проверку
 *  при которой нижняя частота не может быть больше верхней
 */
class PreselectorWidget: public QWidget
{
    Q_OBJECT

    static const quint32 preselectorLowFrequency[];
    static const quint32 preselectorHighFrequency[];
    static const int PRESELECTOR_FREQUENCY_SIZE;

  public:
    PreselectorWidget(QWidget* parent = nullptr);
    void setWidgetData(const QPair<UInt32LowFreq, UInt32HeighFreq>& pair);

    QPair<UInt32LowFreq, UInt32HeighFreq>getPreselectors();
  signals:
    void changed();
  private:
    Q_SLOT void onPreselectorFrequencyChanged(const QString& strFrequency);
  private:
    QComboBox* cbLowFrequency;
    QComboBox* cbHighFrequency;
};

///@}
#endif // PRESELECTOR_WIDGET_H
