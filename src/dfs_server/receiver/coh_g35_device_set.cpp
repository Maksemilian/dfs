#include "coh_g35_device_set.h"

#include "tsip/time_reader.h"
#include "ring_packet_buffer.h"
#include "signal_file_writer.h"
#include "device_set_selector.h"
#include "packet.pb.h"

#include <QTimer>
#include <QDebug>
#include <QDate>

CohG35DeviceSet::CohG35DeviceSet():
    ddc1Buffer(new RingPacketBuffer(16)),
    timeReader(new TimeReader()),
    signalFileWriter(new SignalFileWriter()),
    isFirstBlock(true),
    counterBlockPPS(1),
    currentDDCCounter(-1),
    currentWeekNumber(0),
    currentTimeOfWeek(0)
{

}

QString CohG35DeviceSet::getDeviceSetName(){
    quint32 deviceCount;
    deviceSet->GetDeviceCount(&deviceCount);
    G35DDC_DEVICE_INFO deviceInfo[deviceCount];
    deviceSet->GetDeviceInfos(deviceInfo,&deviceCount);
    QString deviceSetName="DS#";
    for(quint32 i=0;i<deviceCount;i++){
        deviceSetName+=deviceInfo[i].SerialNumber;
        if(i!=deviceCount-1){
            deviceSetName+="_";
        }
    }
    //    G3XDDC_DDC_INFO info;
    //    for (int i=0;i<deviceInfo[0].DDCTypeCount;i++) {
    //        deviceSet->GetDDCInfo(i,&info);
    //        qDebug()<<info.Bandwidth<<info.SampleRate;
    //    }
    return deviceSetName;
}

bool CohG35DeviceSet::setUpDeviceSet(quint32 numberDeviceSet){
    ICohG35DDCDeviceSet *deviceSet=DeviceSetSelector::selectDeviceSet(numberDeviceSet);
    if(deviceSet){
        deviceSet->SetCallback(this);
        this->deviceSet=deviceSet;
        signalFileWriter->createMainDirectory(getDeviceSetName());
        //       signalFileWriter->createWorkDirectory(QDate::currentDate().toString("dd.MM.yyyy"));
        return true;
    }

    return false;
}

void CohG35DeviceSet::resetData(){
    isFirstBlock=true;
    counterBlockPPS=1;
    currentDDCCounter=-1;
    currentWeekNumber=0;
    currentTimeOfWeek=0;
    ddc1Buffer->reset();
}

void CohG35DeviceSet::freeResource(){
    if(deviceSet!=nullptr){
        deviceSet->Release();
        //        delete ddc1Buffer;
    }
}

CohG35DeviceSet::~CohG35DeviceSet()
{
    freeResource();
}

COH_G35DDC_DEVICE_SET CohG35DeviceSet::getDeviceSetInfo(){
    COH_G35DDC_DEVICE_SET deviceSetInfos;
    deviceSet->GetDeviceCount(&deviceSetInfos.DeviceCount);
    deviceSetInfos.DeviceInfo=new G35DDC_DEVICE_INFO[deviceSetInfos.DeviceCount];
    deviceSet->GetDeviceInfos(deviceSetInfos.DeviceInfo,&deviceSetInfos.DeviceCount);
    return deviceSetInfos;
}

//**************COMMAND TO DEVICE SET

bool CohG35DeviceSet::setPower(bool state){
    return deviceSet->SetPower(state);
}

bool CohG35DeviceSet::setAttenuator(unsigned int attenuationLevel){
    return deviceSet->SetAttenuator(attenuationLevel);
}

bool CohG35DeviceSet::setPreamplifierEnabled(bool state){
    return deviceSet->SetPreamp(state);
}

bool CohG35DeviceSet::setPreselectors(unsigned int lowFrequency, unsigned int highFrequency){
    return deviceSet->SetPreselectors(lowFrequency,highFrequency);
}

bool CohG35DeviceSet::setAdcNoiceBlankerEnabled(bool state){
    return deviceSet->SetADCNoiseBlanker(state);
}

bool CohG35DeviceSet::setAdcNoiceBlankerThreshold(unsigned short threshold){
    return deviceSet->SetADCNoiseBlankerThreshold(threshold);
}

bool CohG35DeviceSet::setDDC1Frequency(unsigned int ddc1Frequency){
    return deviceSet->SetDDC1Frequency(ddc1Frequency);
}

bool CohG35DeviceSet::setDDC1Type(quint32 type)
{
    return  deviceSet->SetDDC1(type);
}
bool CohG35DeviceSet::setSettings(const CohG35DeviceSetSettings &settings){
    bool succesed=false;

    succesed=setAttenuator(settings.attenuator);
    qDebug()<<"======Comand  SET_ATTENUATOR "<<settings.attenuator<<"Db"<<"|| Succesed command"<<succesed;

    succesed=setPreselectors(settings.preselectors.first,settings.preselectors.second);
    qDebug()<<"======Comand  SET_PRESELECTORS"<<"Frequency:"<<"Low "<<settings.preselectors.first<<"Hz"
           <<"High "<<settings.preselectors.second<<"Hz"<<"|| Succesed command"<<succesed;

    succesed=setPreamplifierEnabled(settings.preamplifier);
    qDebug()<<"======Comand  SET_PREAMPLIFIED_ENABLED"<<settings.preamplifier<<"|| Succesed command"<<succesed;

    succesed=setAdcNoiceBlankerEnabled(settings.adcEnabled);
    qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_ENABLED"<<settings.adcEnabled<<"|| Succesed command"<<succesed;

    succesed=setAdcNoiceBlankerThreshold(settings.threshold);
    qDebug()<<"======Comand  SET_ADC_NOICE_BLANKER_THRESHOLD"<<settings.threshold<<"|| Succesed command"<<succesed;

    succesed=setDDC1Frequency(settings.frequency);
    qDebug()<<"======Comand  SET_DDC1_FREQUENCY"<<settings.frequency<<"|| Succesed command"<<succesed;

    succesed=deviceSet->SetDDC1(settings.ddcType);
    qDebug()<<"======Comand  SET_DDC1_TYPE"<<settings.ddcType<<"|| Succesed command"<<succesed;

    return succesed;
}

void CohG35DeviceSet::startDDC1(unsigned int samplesPerBuffer,bool writeToFile){
    Q_ASSERT_X(deviceSet,"deviceSet is null","CohG35DeviceSet::startDDC1();");
    if(deviceSet){
        resetData();
        timeReader->start();
        QTimer::singleShot(TIMEOUT,[this,samplesPerBuffer,writeToFile]{
            timeReader->getTime(currentWeekNumber,currentTimeOfWeek);
            deviceSet->StartDDC1(samplesPerBuffer);
            quint32 ddc1Frequency=0;
            if(writeToFile&&deviceSet->GetDDC1Frequency(&ddc1Frequency)){
                qDebug()<<"Start writing file"<<ddc1Frequency;
                signalFileWriter->start(ddc1Buffer.get(),QString::number(ddc1Frequency));
            }
        });
    }
}

void CohG35DeviceSet::stopDDC1(){
    if(deviceSet){
        deviceSet->StopDDC1();
        timeReader->stop();
        ddc1Buffer->reset();
        signalFileWriter->stop();
    }
}

void CohG35DeviceSet::reStartDdc1(unsigned int ddc1TypeIndex,unsigned int samplesPerBuffer,bool writeToFile){
    if(deviceSet){
        bool succesed=true;
        std::shared_ptr<QMetaObject::Connection> sharedPtrConnection(new QMetaObject::Connection) ;
        *sharedPtrConnection=QObject::connect(timeReader,&TimeReader::stopedTrimble,
                                              [this,sharedPtrConnection,ddc1TypeIndex,samplesPerBuffer,writeToFile]{
            QObject::disconnect(*sharedPtrConnection);
            deviceSet->SetDDC1(ddc1TypeIndex);
            quint32 ddcfreq;
            deviceSet->GetDDC1Frequency(&ddcfreq);
            qDebug()<<"RESTART"<<ddcfreq<<writeToFile;
            startDDC1(samplesPerBuffer,writeToFile);
        });
        stopDDC1();
        qDebug()<<"======Comand  SET_DDC1_TYPE"<<ddc1TypeIndex<<"SamplesPerBuffer"<<samplesPerBuffer<<"|| Succesed command"<<succesed;
    }
}
//*********************** ICohG35DDCDeviceSetCallback ************************

void CohG35DeviceSet::CohG35DDC_DDC1StreamCallback(ICohG35DDCDeviceSet *DeviceSet, unsigned int DeviceCount,
                                                   const void **Buffers, unsigned int NumberOfSamples, unsigned int BitsPerSample){
    //qDebug("CALLBACK_BEGIN");
    double ddcSampleCounter;
    quint64 adcPeriodCounter;
    DeviceSet->GetTimestampCounters(&ddcSampleCounter,&adcPeriodCounter);

    bool isChangedDDCCounter=false;

    if(ddcSampleCounter!=currentDDCCounter){
        if(!isFirstBlock){
            //qDebug()<<"*********DDC_NOT_USE"<<ddcSampleCounter<<currentDDCCounter;
        }
        isChangedDDCCounter=true;
        currentDDCCounter=ddcSampleCounter;
        //counterBlockPPS=0;
        //qDebug()<<"*********DDC_NOT_USE"<<ddcSampleCounter<<currentDDCCounter;
    }
    else
    {
        //counterBlockPPS++;
    }

    quint32 timeOfWeek;
    timeReader->getTime(currentWeekNumber,timeOfWeek);
    if(!isFirstBlock)
    {
        if(isChangedDDCCounter)
        {
            currentTimeOfWeek++;
        }

        DDC1StreamCallbackData ddc1StreamCallbackData;
        ddc1StreamCallbackData.DeviceSet=DeviceSet;
        ddc1StreamCallbackData.DeviceCount=DeviceCount;
        ddc1StreamCallbackData.Buffers=Buffers;
        ddc1StreamCallbackData.NumberOfSamples=NumberOfSamples;
        ddc1StreamCallbackData.BitsPerSample=BitsPerSample;

        Packet packet;
        if(ddcSampleCounter>0){
            fillPacket(packet,ddc1StreamCallbackData,ddcSampleCounter,adcPeriodCounter,counterBlockPPS);
            showPacket(packet);
            ddc1Buffer->push(packet);
        }
    }

    if(timeOfWeek>currentTimeOfWeek){
        if(isFirstBlock)
        {
            isFirstBlock=false;

            qDebug()<<"FIRST_TIME"<<timeOfWeek<<currentTimeOfWeek;
        }
        qDebug()<<"*********DDC_NOT_USE"<<ddcSampleCounter<<currentDDCCounter;
        qDebug()<<"CHANGED_TIME"<<timeOfWeek<<currentTimeOfWeek;
        currentTimeOfWeek=timeOfWeek;
    }
    counterBlockPPS++;
    //qDebug("CALLBACK_END");
}

//***********************FILL PACKET***********************

void CohG35DeviceSet::fillPacket(Packet &packet, DDC1StreamCallbackData &ddcStreamCallbackData,
                                 double ddcSampleCounter,unsigned long long adcPeriodCounter,int counterBlockPPS)
{
    quint32 ddc1Frequency,attenuator;
    //В ДЕБАГ РЕЖИМЕ ПРОИСХОДИТ КРАХ ПРОГРАММЫ
    ddcStreamCallbackData.DeviceSet->GetDDC1Frequency(&ddc1Frequency);
    ddcStreamCallbackData.DeviceSet->GetAttenuator(&attenuator);

    quint32 ddc1;
    G3XDDC_DDC_INFO ddcInfo;
    ddcStreamCallbackData.DeviceSet->GetDDC1(&ddc1,&ddcInfo);
    packet.set_block_number(counterBlockPPS);

    packet.set_ddc1_frequency(ddc1Frequency);
    packet.set_attenuator(attenuator);

    packet.set_block_size(static_cast<int>(ddcStreamCallbackData.NumberOfSamples));

    packet.set_device_count(ddcStreamCallbackData.DeviceCount);
    packet.set_sample_rate(ddcInfo.SampleRate);

    packet.set_ddc_sample_counter(ddcSampleCounter);
    packet.set_adc_period_counter(adcPeriodCounter);

    packet.set_week_number(currentWeekNumber);
    packet.set_time_of_week(currentTimeOfWeek);

    if(ddcStreamCallbackData.BitsPerSample==16) {
        qint16 **buffer=reinterpret_cast<qint16**>(const_cast<void **>(ddcStreamCallbackData.Buffers));
        for(quint32 i=0;i<ddcStreamCallbackData.DeviceCount;i++){
            for(quint32 j=0;j<ddcStreamCallbackData.NumberOfSamples*COUNT_SIGNAL_COMPONENT;j++){
                packet.add_sample(buffer[i][j]);
            }
        }

    }else if(ddcStreamCallbackData.BitsPerSample==32){
        qint32 **buffer= reinterpret_cast<qint32**>(const_cast<void **>(ddcStreamCallbackData.Buffers));
        for(quint32 i=0;i<ddcStreamCallbackData.DeviceCount;i++){
            for(quint32 j=0;j<ddcStreamCallbackData.NumberOfSamples*COUNT_SIGNAL_COMPONENT;j++){
                packet.add_sample(buffer[i][j]);
            }
        }
    }
}

//******************OTHER CALLBACKS********************

void CohG35DeviceSet::CohG35DDC_AudioStreamCallback(ICohG35DDCDeviceSet *DeviceSet, unsigned int DeviceIndex, unsigned int Type, const float *Buffer, unsigned int NumberOfSamples)
{   printf("Audio Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug()<<DeviceIndex<<Type<<Buffer[0]<<NumberOfSamples;
}

void CohG35DeviceSet::CohG35DDC_IFCallback(ICohG35DDCDeviceSet *DeviceSet, unsigned int DeviceIndex, const short *Buffer, unsigned int NumberOfSamples, WORD MaxADCAmplitude, unsigned int ADCSamplingRate)
{   printf("IF Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug()<<DeviceIndex<<Buffer[0]<<NumberOfSamples<<MaxADCAmplitude<<ADCSamplingRate;
}

void CohG35DeviceSet::CohG35DDC_DDC2PreprocessedStreamCallback(ICohG35DDCDeviceSet *DeviceSet, unsigned int DeviceIndex, const float *Buffer, unsigned int NumberOfSamples, float SlevelPeak, float SlevelRMS)
{   printf("DDC2 PreprocessedStream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug()<<DeviceIndex<<Buffer[0]<<NumberOfSamples<<SlevelPeak<<SlevelRMS;
}

void CohG35DeviceSet::CohG35DDC_DDC2StreamCallback(ICohG35DDCDeviceSet *DeviceSet, unsigned int DeviceIndex, const float *Buffer, unsigned int NumberOfSamples)
{   printf("DDC2 Stream\n");
    BOOL power;
    DeviceSet->GetPower(&power);
    qDebug()<<DeviceIndex<<Buffer[0]<<NumberOfSamples;
}

void CohG35DeviceSet::showPacket(Packet &packet){

    qDebug()<<"DDC_CALLBACK:"
           <<packet.block_number()
          <<packet.block_size()
         <<packet.sample_rate()
           ///<<packet.ByteSize()
        <<"||"<<"DDC_C"<<packet.ddc_sample_counter()<<"ADC_C"<<packet.adc_period_counter()
       <<"||"<<"WN"<<packet.week_number()<<"TOW:"<<packet.time_of_week();
}
