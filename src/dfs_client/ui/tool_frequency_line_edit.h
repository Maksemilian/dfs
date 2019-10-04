#ifndef FREQUENCY_LINE_EDIT_H
#define FREQUENCY_LINE_EDIT_H


#include <QLineEdit>

class FrequencyLineEdit : public QLineEdit/*,public IToolBarWidget*/
{
    Q_OBJECT
    typedef enum
    {
        FrequencyEditUnits_Hz=0,
        FrequencyEditUnits_kHz=1,
        FrequencyEditUnits_MHz=2,
        FrequencyEditUnits_GHz=3,
        FrequencyUnitEnd=4
    } TFrequencyEditUnits;

    struct Freq
    {
        QString                Abbr;
        QString              Text;
        quint32              Mul;
        TFrequencyEditUnits  Code;
        QString              defaultText;
    };
    static const Freq Units[4];
    static const double MIN_VALUE_IN_HZ;
    static const double MAX_VALUE_IN_HZ;

public:
    FrequencyLineEdit(/*MainWindow*mainWindow,*/QWidget*parent=nullptr);

    void setFrequencyValueInHz(quint32 frequency);
    quint32 getFrequencyValueInHz();

    static QString formatingFrequencyToString(quint32 frequency);
    static quint32 formatingStringFrequencyToNumber(const QString &strFrequency);

    Q_SIGNAL void frequencyChanged(quint32 ddc1FrequencyHz);
signals:
    void changed();
protected :
    void keyPressEvent(QKeyEvent *event)override;
private:
    void onChangedPositionCursor(int oldV , int newV);
    void onReturnPresed();

    int postfixLength=0;
    int unit;
};

#endif // FREQUENCY_LINE_EDIT_H
