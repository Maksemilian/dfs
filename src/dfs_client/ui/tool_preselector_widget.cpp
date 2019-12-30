#include "tool_preselector_widget.h"
#include "tool_frequency_line_edit.h"

#include <QHBoxLayout>
#include <QComboBox>

const int PreselectorWidget::PRESELECTOR_FREQUENCY_SIZE = 5;

const quint32 PreselectorWidget::preselectorLowFrequency[PreselectorWidget::PRESELECTOR_FREQUENCY_SIZE] =
{
    0,
    850'000,
    2'400'000,
    5'400'000,
    11'800'000
};

const quint32 PreselectorWidget::preselectorHighFrequency[PreselectorWidget::PRESELECTOR_FREQUENCY_SIZE] =
{
    3'100'000,
    5'400'000,
    11'800'000,
    23'300'000,
    50'000'000
};

PreselectorWidget::PreselectorWidget(/*MainWindow*mainWindow,*/QWidget* parent)
    : QWidget(parent) /*,IToolBarWidget(mainWindow)*/
{
    setLayout(new QHBoxLayout);
    cbLowFrequency = new QComboBox(this);

    connect(cbLowFrequency, static_cast<void(QComboBox::*)(const QString& strLowFrequency)>(&QComboBox::activated),
            this, &PreselectorWidget::onPreselectorFrequencyChanged);

    //preseector HIGH FREQUENCY COMBO BOX
    cbHighFrequency = new QComboBox(this);

    connect(cbHighFrequency, static_cast<void(QComboBox::*)(const QString& strHighFrequency)>(&QComboBox::activated),
            this, &PreselectorWidget::onPreselectorFrequencyChanged);

    for(int i = 0; i < PRESELECTOR_FREQUENCY_SIZE; i++)
    {
        cbLowFrequency->addItem(FrequencyLineEdit::formatingFrequencyToString(preselectorLowFrequency[i]));
        cbHighFrequency->addItem(FrequencyLineEdit::formatingFrequencyToString(preselectorHighFrequency[i]));
    }
    layout()->addWidget(cbLowFrequency);
    layout()->addWidget(cbHighFrequency);
}

void PreselectorWidget::setWidgetData(const QPair<UInt32LowFreq, UInt32HeighFreq>& pair)
{
    cbLowFrequency->setCurrentText (FrequencyLineEdit::formatingFrequencyToString(pair.first));
    cbHighFrequency->setCurrentText(FrequencyLineEdit::formatingFrequencyToString(pair.second));
}

QPair<UInt32LowFreq, UInt32HeighFreq> PreselectorWidget::getPreselectors()
{
    return
    {
        FrequencyLineEdit::formatingStringFrequencyToNumber(cbLowFrequency->currentText()),
        FrequencyLineEdit:: formatingStringFrequencyToNumber(cbHighFrequency->currentText())
    };
}

void PreselectorWidget::onPreselectorFrequencyChanged(const QString& strFrequency)
{
    if(!strFrequency.isEmpty())
        changed();
}
