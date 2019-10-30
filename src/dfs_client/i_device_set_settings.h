#ifndef IGETSETTINGS_H
#define IGETSETTINGS_H

#include <QObject>

using Preselectors= QPair<quint32,quint32>;

struct DeviceSetSettings
{
    quint32 attenuator;
    Preselectors preselectors;
    bool preamplifier;
    bool adcEnabled;
    quint16 threshold;
    quint32 frequency;
    quint32 ddcType;
    quint32 samplesPerBuffer;
    bool powerEnabled;
};

Q_DECLARE_METATYPE(DeviceSetSettings);


class IDeviceSetSettings
{
public :
    virtual bool getPower()=0;
    virtual DeviceSetSettings getSettings()=0;
    virtual quint32 getDDC1Frequency()=0;
    virtual quint32 getDDC1Type()=0;
    virtual quint32 getSamplesPerBuffer()=0;
    virtual quint32 getBandwith()=0;
    virtual quint32 getSampleRateForBandwith()=0;
    virtual quint32 getAttenuator()=0;
    virtual bool getPreamplifierEnabled()=0;
    virtual Preselectors getPreselectors()=0;

    virtual bool getAdcNoiceBlankerEnabled()=0;
    virtual quint16 getAdcNoiceBlankerThreshold()=0;

};
#endif // IGETSETTINGS_H
