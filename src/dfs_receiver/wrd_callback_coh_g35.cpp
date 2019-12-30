#include "wrd_callback_coh_g35.h"

#include "trmbl_tsip_reader.h"

#include <QTimer>
#include <QDebug>


//*********************** ICohG35DDCDeviceSetCallback ************************

CohG35Callback::CohG35Callback(const ShPtrRingPacketBuffer& buffer, TimeReader& timeReader):
    buffer(buffer),
    timeReader(timeReader),
    isFirstBlock(true),
    counterBlockPPS(1),
    currentDDCCounter(-1),
    currentWeekNumber(0),
    currentTimeOfWeek(0)
{

}

void CohG35Callback::resetData()
{
    isFirstBlock = true;
    counterBlockPPS = 1;
    currentDDCCounter = -1;
    currentWeekNumber = 0;
    currentTimeOfWeek = 0;
    buffer->reset();

    timeReader.getTime(currentWeekNumber, currentTimeOfWeek);
}

void CohG35Callback::CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceCount,
        const void** Buffers, unsigned int NumberOfSamples, unsigned int BitsPerSample)
{
    //qDebug("CALLBACK_BEGIN");
    double ddcSampleCounter;
    quint64 adcPeriodCounter;
    DeviceSet->GetTimestampCounters(&ddcSampleCounter, &adcPeriodCounter);

    bool isChangedDDCCounter = false;

    if(ddcSampleCounter != currentDDCCounter)
    {
        if(!isFirstBlock)
        {
            //qDebug()<<"*********DDC_NOT_USE"<<ddcSampleCounter<<currentDDCCounter;
        }
        isChangedDDCCounter = true;
        currentDDCCounter = ddcSampleCounter;
        //counterBlockPPS=0;
        //qDebug()<<"*********DDC_NOT_USE"<<ddcSampleCounter<<currentDDCCounter;
    }
    else
    {
        //counterBlockPPS++;
    }

    quint32 timeOfWeek;
    timeReader.getTime(currentWeekNumber, timeOfWeek);
    if(!isFirstBlock)
    {
        if(isChangedDDCCounter)
        {
            currentTimeOfWeek++;
        }

        DDC1StreamCallbackData ddc1StreamCallbackData;
        ddc1StreamCallbackData.DeviceSet = DeviceSet;
        ddc1StreamCallbackData.DeviceCount = DeviceCount;
        ddc1StreamCallbackData.Buffers = Buffers;
        ddc1StreamCallbackData.NumberOfSamples = NumberOfSamples;
        ddc1StreamCallbackData.BitsPerSample = BitsPerSample;

        //        Packet packet;
        //        if(ddcSampleCounter>0){
        //            fillPacket(packet,ddc1StreamCallbackData,ddcSampleCounter,adcPeriodCounter,counterBlockPPS);
        //            showPacket(packet);
        //            ddc1Buffer->push(packet);
        //        }
        proto::receiver::Packet packet;
        if(ddcSampleCounter > 0)
        {
            fillPacket(packet, ddc1StreamCallbackData, ddcSampleCounter, adcPeriodCounter, counterBlockPPS);
            showPacket(packet);
            buffer->push(packet);
        }
    }

    if(timeOfWeek > currentTimeOfWeek)
    {
        if(isFirstBlock)
        {
            isFirstBlock = false;

            qDebug() << "FIRST_TIME" << timeOfWeek << currentTimeOfWeek;
        }
        qDebug() << "*********DDC_NOT_USE" << ddcSampleCounter << currentDDCCounter;
        qDebug() << "CHANGED_TIME" << timeOfWeek << currentTimeOfWeek;
        currentTimeOfWeek = timeOfWeek;
    }
    counterBlockPPS++;
    //qDebug("CALLBACK_END");
}

//***********************FILL PACKET***********************

void CohG35Callback::fillPacket(proto::receiver::Packet& packet,
                                DDC1StreamCallbackData& ddcStreamCallbackData,
                                double ddcSampleCounter,
                                unsigned long long adcPeriodCounter,
                                int counterBlockPPS)
{
    quint32 ddc1Frequency, attenuator;
    //В ДЕБАГ РЕЖИМЕ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
    ddcStreamCallbackData.DeviceSet->GetDDC1Frequency(&ddc1Frequency);
    ddcStreamCallbackData.DeviceSet->GetAttenuator(&attenuator);

    quint32 ddc1;
    G3XDDC_DDC_INFO ddcInfo;
    ddcStreamCallbackData.DeviceSet->GetDDC1(&ddc1, &ddcInfo);
    packet.set_block_number(counterBlockPPS);

    packet.set_ddc1_frequency(ddc1Frequency);
    packet.set_attenuator(attenuator);

    packet.set_block_size(ddcStreamCallbackData.NumberOfSamples);

    packet.set_device_count(ddcStreamCallbackData.DeviceCount);
    packet.set_sample_rate(ddcInfo.SampleRate);

    packet.set_ddc_sample_counter(ddcSampleCounter);
    packet.set_adc_period_counter(adcPeriodCounter);

    packet.set_week_number(currentWeekNumber);
    packet.set_time_of_week(currentTimeOfWeek);
    quint8 COUNT_SIGNAL_COMPONENT = 2;
    if(ddcStreamCallbackData.BitsPerSample == 16)
    {
        qint16** buffer = reinterpret_cast<qint16**>(const_cast<void**>(ddcStreamCallbackData.Buffers));
        for(quint32 i = 0; i < ddcStreamCallbackData.DeviceCount; i++)
        {
            for(quint32 j = 0; j < ddcStreamCallbackData.NumberOfSamples * COUNT_SIGNAL_COMPONENT; j++)
            {
                packet.add_sample(buffer[i][j]);
            }
        }

    }
    else if(ddcStreamCallbackData.BitsPerSample == 32)
    {
        qint32** buffer = reinterpret_cast<qint32**>(const_cast<void**>(ddcStreamCallbackData.Buffers));
        for(quint32 i = 0; i < ddcStreamCallbackData.DeviceCount; i++)
        {
            for(quint32 j = 0; j < ddcStreamCallbackData.NumberOfSamples * COUNT_SIGNAL_COMPONENT; j++)
            {
                packet.add_sample(buffer[i][j]);
            }
        }
    }
}

void CohG35Callback::showPacket(proto::receiver::Packet& packet)
{

    qDebug() << "DDC_CALLBACK:"
             << packet.block_number()
             << packet.block_size()
             << packet.sample_rate()
             ///<<packet.ByteSize()
             << "||" << "DDC_C" << packet.ddc_sample_counter() << "ADC_C" << packet.adc_period_counter()
             << "||" << "WN" << packet.week_number() << "TOW:" << packet.time_of_week();
}

//******************OTHER CALLBACKS********************

void CohG35Callback::CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, unsigned int Type, const float* Buffer, unsigned int NumberOfSamples)
{
    printf("Audio Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug() << DeviceIndex << Type << Buffer[0] << NumberOfSamples;
}

void CohG35Callback::CohG35DDC_IFCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const short* Buffer, unsigned int NumberOfSamples, WORD MaxADCAmplitude, unsigned int ADCSamplingRate)
{
    printf("IF Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug() << DeviceIndex << Buffer[0] << NumberOfSamples << MaxADCAmplitude << ADCSamplingRate;
}

void CohG35Callback::CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const float* Buffer, unsigned int NumberOfSamples, float SlevelPeak, float SlevelRMS)
{
    printf("DDC2 PreprocessedStream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug() << DeviceIndex << Buffer[0] << NumberOfSamples << SlevelPeak << SlevelRMS;
}

void CohG35Callback::CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet* DeviceSet, unsigned int DeviceIndex, const float* Buffer, unsigned int NumberOfSamples)
{
    printf("DDC2 Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug() << DeviceIndex << Buffer[0] << NumberOfSamples;
}

