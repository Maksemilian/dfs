#include "tool_frequency_line_edit.h"

#include <QKeyEvent>
#include <QDebug>
#include <iostream>
/*! \addtogroup client
 */
///@{
using namespace std;

const FrequencyLineEdit::Freq FrequencyLineEdit::Units[4] =
{
    {"H", "Hz", 1, FrequencyEditUnits_Hz, "1000"},
    {"k", "kHz", 1000, FrequencyEditUnits_kHz, "1.000"},
    {"M", "MHz", 1000000, FrequencyEditUnits_MHz, "0.001000"},
    {"G", "GHz", 1000000000, FrequencyEditUnits_GHz, "0.000001000"}
};

const double FrequencyLineEdit::MIN_VALUE_IN_HZ = 1000.0;
const double FrequencyLineEdit::MAX_VALUE_IN_HZ = 50000000.0;

FrequencyLineEdit::FrequencyLineEdit(QWidget* parent):
    QLineEdit (parent)
{
    QFont font;
    font.setPixelSize(40);
    setFixedHeight(40);
    setFont(font);

    setAlignment(Qt::AlignRight);
    setCursorMoveStyle(Qt::VisualMoveStyle);
    unit = FrequencyEditUnits_MHz;
    setText(Units[unit].Text);

    postfixLength = 3;
    setCursorPosition(0);

    setFixedWidth(250);
    connect(this, &FrequencyLineEdit::cursorPositionChanged, this, &FrequencyLineEdit::onChangedPositionCursor);
}

QString FrequencyLineEdit::formatingFrequencyToString(quint32 frequency)
{
    QString formatingString;
    double f = frequency;
    if(f < Units[FrequencyEditUnits_kHz].Mul)
    {
        formatingString = QString::number(f) + " " + Units[FrequencyEditUnits_Hz].Text;
    }
    else
    {
        f /= Units[FrequencyEditUnits_kHz].Mul;
        if(f < Units[FrequencyEditUnits_kHz].Mul)
        {
            formatingString = QString::number(f) + " " + Units[FrequencyEditUnits_kHz].Text;
        }
        else
        {
            f /= Units[FrequencyEditUnits_kHz].Mul;
            formatingString = QString::number(f) + " " + Units[FrequencyEditUnits_MHz].Text;
        }
    }
    return formatingString;
}

quint32 FrequencyLineEdit::formatingStringFrequencyToNumber(const QString& strFrequency)
{
    quint32 formatingNumber = 0;
    QStringList splitStr = strFrequency.split(' ');
    double f = splitStr[0].toDouble();
    if(splitStr[1] == Units[FrequencyEditUnits_kHz].Text)
    {
        f *= Units[FrequencyEditUnits_kHz].Mul;
    }
    else if(splitStr[1] == Units[FrequencyEditUnits_MHz].Text)
    {
        f *= Units[FrequencyEditUnits_MHz].Mul;
    }
    formatingNumber = static_cast<quint32>(f);
    return formatingNumber;
}

void FrequencyLineEdit::setFrequencyValueInHz(quint32 frequency)
{
    double dblFrequency = static_cast<double>(frequency);

    dblFrequency /= Units[unit].Mul;

    setText(QString::number(dblFrequency) + Units[unit].Text);
    setCursorPosition(text().length() - Units[unit].Text.length());
}

quint32 FrequencyLineEdit::getFrequencyValueInHz()
{

    double value = text().left(text().length() - Units[unit].Text.length()).toDouble();
    value *= Units[unit].Mul;
    if(value <= MIN_VALUE_IN_HZ || value >= MAX_VALUE_IN_HZ)
    {
        value = Units[FrequencyEditUnits_kHz].Mul;
    }
    //TODO РАЗОБРАТЬСЯ КАК ПРОИСХОДИТ ПОЛУЧЕНИЕ ПОСЛЕ ЗАПЯТОЙ 9999
    return static_cast<quint32>(value + 0.5);
}

void FrequencyLineEdit::onChangedPositionCursor(int oldV, int newV)
{
    Q_UNUSED(oldV);

    if(text().length() - postfixLength < newV)
    {
        setCursorPosition(text().length() - Units[unit].Text.length());
    }
}

void FrequencyLineEdit::onReturnPresed()
{
    int index = 0;
    for(int i = 0; i < FrequencyUnitEnd; i++)
    {
        if(Units[i].Code == unit)
        {
            index = unit;
        }
    }
    QString strNumber = text().left(text().length() - Units[index].Text.length());

    bool isEmptyString = false;
    bool isNotValidRange = false;
    double val = 0.0;

    if(strNumber.isEmpty())
    {
        isEmptyString = true;
    }
    else
    {
        val = strNumber.toDouble() * Units[index].Mul;
        if(val <= MIN_VALUE_IN_HZ || val >= MAX_VALUE_IN_HZ)
        {
            isNotValidRange = true;
            strNumber.clear();
        }
    }
    if(isEmptyString || isNotValidRange)
    {
        strNumber.append(Units[unit].defaultText);
        val = strNumber.toDouble() * Units[index].Mul;
    }

    setText(strNumber + Units[index].Text);
    setCursorPosition(text().length() - Units[index].Text.length());

    if(static_cast<quint32>(val) > 0)
    {
        changed();
    }
}

void FrequencyLineEdit::keyPressEvent(QKeyEvent* event)
{
    static bool isPoint = false;
    //qDebug()<<"KeyPressEvent"<<cursorPosition()<<event->key()<<(long long)Qt::Key_Return<<(long long)Qt::Key_Period;

    QString currentText = text();
    int pos = cursorPosition();
    if(event->key() == Qt::Key_Period && !isPoint)
    {
        currentText.insert(pos, event->key());
        setText(currentText);
        setCursorPosition(pos + 1);
        isPoint = true;
    }
    if(event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
    {
        currentText.insert(pos, event->key());
        setText(currentText);
        setCursorPosition(pos + 1);
    }
    else
    {
        switch (event->key())
        {
            case Qt::Key_Backspace:
                if(currentText.at(pos - 1).isDigit() || currentText.at(pos - 1) == Qt::Key_Period)
                {
                    if(currentText.at(pos - 1) == Qt::Key_Period)
                    {
                        //qDebug("Rem Point");
                        isPoint = false;
                    }
                    //qDebug()<<"Backspace"<<pos<<currentText;
                    currentText.remove(pos - 1, 1);
                    setText(currentText);
                    setCursorPosition(pos - 1);
                }
                break;
            case Qt::Key_Delete:

                if(currentText.at(pos).isDigit() || currentText.at(pos) == Qt::Key_Period)
                {
                    if(currentText.at(pos) == Qt::Key_Period)
                    {
                        //qDebug("Rem Point");
                        isPoint = false;
                    }
                    //qDebug()<<"Delete"<<pos<<currentText;
                    currentText.remove(pos, 1);
                    setText(currentText);
                    setCursorPosition(pos);
                }
                break;
            case Qt::Key_Left:
                setCursorPosition(pos - 1);
                break;
            case Qt::Key_Right:
                if(currentText.at(pos).isDigit())
                {
                    setCursorPosition(pos + 1);
                }
                break;
            case Qt::Key_Return:
                //qDebug("enter");
                onReturnPresed();
                break;
            case Qt::Key_G:
                setText(currentText.left(currentText.length() - postfixLength) +
                        Units[FrequencyEditUnits_GHz].Text);
                postfixLength = Units[FrequencyEditUnits_GHz].Text.length();
                setCursorPosition(pos);
                unit = FrequencyEditUnits_GHz;
                break;
            case Qt::Key_M:
                setText(currentText.left(currentText.length() - postfixLength) +
                        Units[FrequencyEditUnits_MHz].Text);
                postfixLength = Units[FrequencyEditUnits_MHz].Text.length();
                setCursorPosition(pos);
                unit = FrequencyEditUnits_MHz;
                break;
            case Qt::Key_K:
                setText(currentText.left(currentText.length() - postfixLength) +
                        Units[FrequencyEditUnits_kHz].Text);
                postfixLength = Units[FrequencyEditUnits_kHz].Text.length();
                setCursorPosition(pos);
                unit = FrequencyEditUnits_kHz;
                break;
            case Qt::Key_H:
                setText(currentText.left(currentText.length() - postfixLength) +
                        Units[FrequencyEditUnits_Hz].Text);
                postfixLength = Units[FrequencyEditUnits_Hz].Text.length();
                setCursorPosition(pos);
                unit = FrequencyEditUnits_Hz;
                break;
        }
    }
}
///@}
