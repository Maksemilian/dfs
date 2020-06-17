#ifndef WIDGETS_H
#define WIDGETS_H

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>


/*! \addtogroup client
 */


class ToolBarLineEdit: public QLineEdit
{
    Q_OBJECT
  public:
    ToolBarLineEdit(QWidget* parent = nullptr);
    quint16 getValue();
  signals:
    void changed();
};


class ToolBarComboBox: public QComboBox
{
    Q_OBJECT
  public:
    ToolBarComboBox(QWidget* parent = nullptr);
  signals:
    void changed();
};

///@{
using UInt32Bandwth = quint32;
using Uint32SampleRate = quint32;
/*!
 * \brief Виджет для ввода полосы пропускания.
 */
class BandwithComboBox: public ToolBarComboBox
{
    static const int BANDWITH_SIZE;
    static const quint32 bandwithArray[];
  public:
    BandwithComboBox(QWidget* parent = nullptr);
    static  QMap<UInt32Bandwth, Uint32SampleRate>bandwithAndSampleRateMap();
    inline quint32 getCurrentBandwith()
    {
        return bandwithArray[currentIndex()];
    }
    quint32 currentTypeIndex();

};

/*!
 * \brief Виджет для ввода размера блока DDC1.
 */
class SamplePerBufferComboBox: public ToolBarComboBox
{
    static const int SAMPLES_PER_BUFFER_SIZE;
    static const quint16 samplesPerBufferArray[];
  public:
    SamplePerBufferComboBox(QWidget* parent = nullptr);
    quint32 samplesPerBuffer();
};

/*!
 * \brief Виджет для ввода данных аттенюатора.
 */
class AttenuatorComboBox: public ToolBarComboBox
{
    static const int ATTENUATOR_SIZE;
    static const quint8 attenuatorArray[];
  public:
    AttenuatorComboBox(QWidget* parent = nullptr);
    quint32 getAttenuationLevel();
};
///@}

#endif // WIDGETS_H
