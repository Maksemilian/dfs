#include "main_window_settings.h"

#include <QDebug>

MainWindowSettings::MainWindowSettings(const QString &filePath):
    QSettings(filePath,QSettings::IniFormat)
{
}

//ReceiverSettings MainWindowSettings::load(){
MainWindowSettings::Data MainWindowSettings::load(){
    MainWindowSettings::Data settings;
    beginGroup("attenuator");
    settings.attenuatorEnable=value("enable").toBool();
    settings.attenuator=value("attenuation_level_db").toUInt();
    endGroup();

    beginGroup("preselectors");
    QPair<quint32,quint32>preselectors;
    preselectors.first=value("low_frequency").toUInt();
    preselectors.second=value("high_frequency").toUInt();
    settings.preselectorsEnable=value("enable").toBool();
    settings.preselectors=preselectors;
    endGroup();

    beginGroup("preamplifier");
    settings.preamplifierEnable=value("enabled").toBool();
    endGroup();

    beginGroup("adc_noice_blanker");
    QPair<quint32,quint32>adcNoiceBlanker;
    adcNoiceBlanker.first=value("enabled").toBool();
    adcNoiceBlanker.second=value("threshold").toUInt();
    settings.adcNoiceBlanker=adcNoiceBlanker;
    endGroup();

    beginGroup("ddc1");
    settings.ddc1Frequency=value("frequency").toUInt();
    settings.ddc1TypeIndex=value("type_index").toUInt();
    settings.samplesPerBuffer=value("samples_per_buffer").toUInt();
    endGroup();
    return settings;
}

//void MainWindowSettings::save(const ReceiverSettings &settings){
void MainWindowSettings::save(const MainWindowSettings::Data &settings){

    beginGroup("attenuator");
    setValue("enable",settings.attenuatorEnable);
    setValue("attenuation_level_db",settings.attenuator);
    endGroup();

    beginGroup("preselectors");
    setValue("enable",settings.preselectorsEnable);
    setValue("low_frequency",settings.preselectors.first);
    setValue("high_frequency",settings.preselectors.second);
    endGroup();

    beginGroup("preamplifier");
    setValue("enabled",settings.preamplifierEnable);
    endGroup();

    beginGroup("adc_noice_blanker");
    setValue("enabled",settings.adcNoiceBlanker.first);
    setValue("threshold",settings.adcNoiceBlanker.second);
    endGroup();

    beginGroup("ddc1");
    setValue("frequency",settings.ddc1Frequency);
    setValue("type_index",settings.ddc1TypeIndex);
    setValue("samples_per_buffer",settings.samplesPerBuffer);
    endGroup();
    sync();
}

void MainWindowSettings::show(){
//    ReceiverSettings receiverSettings=load();
    Data receiverSettings=load();
    qDebug()<<"Att"<<receiverSettings.attenuator;
    qDebug()<<"Pres"<<receiverSettings.preselectors;
    qDebug()<<"Preamp"<<receiverSettings.preamplifierEnable;
    qDebug()<<"ADC"<<receiverSettings.adcNoiceBlanker;
    qDebug()<<"DDC1Freq"<<receiverSettings.ddc1Frequency;
    qDebug()<<"DDC1TI"<<receiverSettings.ddc1TypeIndex;
    qDebug()<<"SPB"<<receiverSettings.samplesPerBuffer;
}
