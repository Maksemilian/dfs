#include "tool_widgets.h"
#include "tool_frequency_line_edit.h"

const int BandwithComboBox::BANDWITH_SIZE=12;//MAX BANDWITH SIZE = 33 IT DO NOT USE;

const quint32 BandwithComboBox::bandwithArray[BandwithComboBox::BANDWITH_SIZE]=
{
    20'000,
    24'000,
    32'000,
    40'000,
    50'000,
    64'000,
    80'000,
    100'000,
    125'000,
    160'000,
    200'000,
    250'000
    // 320000,400000,500000,640000,800000,
    //    1000000,1250000,1600000,2000000,2500000,3200000,4000000,5000000,6400000,8000000,
    //    10000000,12500000,16000000,20000000,25000000,32000000
};

QMap<UInt32Bandwth,Uint32SampleRate> BandwithComboBox::bandwithAndSampleRateMap()
{
    QMap<UInt32Bandwth,Uint32SampleRate>map;
    map[20'000]=25'000;
    map[24'000]=32'000;
    map[32'000]=40'000;
    map[40'000]=50'000;
    map[50'000]=62'500;
    map[64'000]=80'000;
    map[80'000]=100'000;
    map[100'000]=125'000;
    map[125'000]=160'000;
    map[160'000]=200'000;
    map[200'000]=250'000;
    map[250'000]=312'500;

    return map;
}

const int SampleRateComboBox::SAMPLES_PER_BUFFER_SIZE=4;

const quint16 SampleRateComboBox::samplesPerBufferArray[SampleRateComboBox::SAMPLES_PER_BUFFER_SIZE]=
{
    1024,
    2048,
    4096,
    8192
};

const int AttenuatorComboBox::ATTENUATOR_SIZE=7;

const quint8 AttenuatorComboBox::attenuatorArray[AttenuatorComboBox::ATTENUATOR_SIZE]=
{
    3,
    6,
    9,
    12,
    15,
    18,
    21
};

ToolBarLineEdit::ToolBarLineEdit(/*MainWindow*mainWindow,*/QWidget*parent)
    :QLineEdit(parent)/*,IToolBarWidget (mainWindow)*/
{
    connect(this,&QLineEdit::returnPressed,[this]
    {
        changed();
    });
}

quint16 ToolBarLineEdit::getValue()
{
    return  text().toUShort();
}

ToolBarComboBox::ToolBarComboBox(/*MainWindow*mainWindow,*/QWidget*parent)
    :QComboBox(parent)/*,IToolBarWidget(mainWindow)*/
{
    setEditable(false);
    connect(this,static_cast<void(QComboBox::*)(const QString &text)>(&QComboBox::activated),
            [this](const QString &text){
        if(!text.isEmpty())
           emit changed();
    });
}


BandwithComboBox::BandwithComboBox(/*MainWindow*mainWindow,*/QWidget*parent)
    :ToolBarComboBox(/*mainWindow,*/parent)
{
    for(int i=0;i<BANDWITH_SIZE;i++)
        addItem(FrequencyLineEdit::formatingFrequencyToString(bandwithArray[i]));

    int maxVisibleItems= BANDWITH_SIZE*80/100;
    setMaxVisibleItems(maxVisibleItems);
    setCurrentIndex(0);

//    QObject::connect(this,
//                     static_cast<void(QComboBox::*)(const QString &text)>(&QComboBox::activated),
//                     [this](const QString &text){
//        if(!text.isEmpty()){
//            this->changed();
//        }
//    });
}

quint32 BandwithComboBox::currentTypeIndex()
{
    return static_cast<quint32>(currentIndex());
}

SampleRateComboBox::SampleRateComboBox(/*MainWindow*mainWindow,*/QWidget*parent)
    :ToolBarComboBox (/*mainWindow,*/parent)
{
    for(int i=0;i<SAMPLES_PER_BUFFER_SIZE;i++)
        addItem(QString::number(samplesPerBufferArray[i]));

    setCurrentIndex(SAMPLES_PER_BUFFER_SIZE-1);
//    QObject::connect(this,
//                     static_cast<void(QComboBox::*)(const QString &text)>(&QComboBox::activated),
//                     [this](const QString &text){
//        if(!text.isEmpty()){
//            this->changed();
//        }
//    });
}

quint32 SampleRateComboBox::samplesPerBuffer()
{
    return currentText().toUInt();
}

AttenuatorComboBox::AttenuatorComboBox(/*MainWindow*mainWindow,*/QWidget*parent)
    :ToolBarComboBox (/*mainWindow,*/parent)
{
    for(int i=0;i<ATTENUATOR_SIZE;i++)
        addItem(QString::number(attenuatorArray[i])+" Db");
}

quint32 AttenuatorComboBox::getAttenuationLevel()
{
    QString attetuatorText=currentText();
    //TODO СДЕЛАТЬ ПРОВЕРКУ
    return  attetuatorText.left(attetuatorText.indexOf(' ')).toUInt();
}
