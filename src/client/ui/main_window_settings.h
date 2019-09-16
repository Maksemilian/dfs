#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class MainWindowSettings :public QSettings
{
public:
    struct Data{
        bool attenuatorEnable;
        quint32 attenuator;

        bool preselectorsEnable;
        QPair<quint32,quint32>preselectors;

        bool preamplifierEnable;

        QPair<bool,quint16>adcNoiceBlanker;

        quint32 ddc1Frequency;
        quint32 ddc1TypeIndex;
        quint32 samplesPerBuffer;
    };

public:
    MainWindowSettings(const QString &filePath);
    //    ReceiverSettings load();
    //    void save(const ReceiverSettings &receiverSettings);
    MainWindowSettings::Data load();
    void save(const MainWindowSettings::Data &receiverSettings);
    void show();
};

#endif // SETTINGS_H
