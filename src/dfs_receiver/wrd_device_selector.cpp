#include "wrd_device_selector.h"
#include "wrd_coh_g35_ds.h"
#include "wrd_g35_d.h"

#include "G35DDCAPI.h"
#include <QDebug>

extern G3XDDCAPI_CREATE_INSTANCE createInstance;

ShPtrDevice CohG35DeviceCreator::createDevice(unsigned int deviceSetIndex,
                                              const ShPtrRingBuffer &buffer)
{
    ICohG35DDCDeviceSetEnumerator *enumerator;
    ICohG35DDCDeviceSet *deviceSet;

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"Enumerator error";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET,
                       reinterpret_cast<void**>(&deviceSet))){
        qDebug()<<"DeviceSet error";
        return nullptr;
    }

    quint32 countDeviceInSet=0;
    enumerator->GetDeviceSetInfo(deviceSetIndex,nullptr,&countDeviceInSet);

    G35DDC_DEVICE_INFO *deviceInfoMas=new G35DDC_DEVICE_INFO[countDeviceInSet];
    enumerator->GetDeviceSetInfo(deviceSetIndex,deviceInfoMas,&countDeviceInSet);

    if(deviceSet->Open(deviceInfoMas,countDeviceInSet)){
        qDebug()<<"Device Set"<<deviceSetIndex<<"- count device:"<<countDeviceInSet;
        for(quint32 deviceIndex=0;deviceIndex<countDeviceInSet;deviceIndex++)
            qDebug()<<"|_____SerialNumber:" <<deviceInfoMas[deviceIndex].SerialNumber;

        delete []deviceInfoMas;
        enumerator->Release();
        //        return deviceSet;
        std::shared_ptr<CohG35DeviceSet> shPtr= std::make_shared<CohG35DeviceSet>(deviceSet);
        shPtr->setCallback(CallbackFactory::cohG35CallbackInstance(buffer));
        return static_cast<std::shared_ptr<IDevice>>(shPtr);
    }
    qDebug()<<"Device is not open";
    delete [] deviceInfoMas;

    enumerator->Release();
    deviceSet->Release();
    return nullptr;
}

ShPtrDevice SingleG35DeviceCreator::createDevice(unsigned int deviceIndex,
                                                 const ShPtrRingBuffer &buffer)
{
    IG35DDCDevice *device=nullptr;
    IG35DDCDeviceEnumerator *enumerator=nullptr;
    G35DDC_DEVICE_INFO devInfo;

    if(!createInstance(G35DDC_CLASS_ID_DEVICE_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"FAILED ENUMERATOR INSTANCE";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_DEVICE,
                       reinterpret_cast<void**>(&device))){
        qDebug()<<"FAILED DEVICE INSTANCE";
        return nullptr;
    }

    enumerator->Enumerate();

    quint32 count=enumerator->GetCount();

    if(count!=0&&deviceIndex<count)
    {
        printf("Available G35DDC devices count=%d:\n",count);

        enumerator->GetDeviceInfo(deviceIndex,&devInfo,sizeof(devInfo));
        printf("%d. SN: %s\n",deviceIndex,devInfo.SerialNumber);
    }
    else
    {
        printf("No available G35DDC device found.\n");
        return nullptr;
    }

    enumerator->Release();

    //Opening the first available G35DDC device using predefined G3XDDC_OPEN_FIRST constant
    if(device->Open(devInfo.DevicePath))
    {
        qDebug()<<"Device:"<<deviceIndex<<devInfo.SerialNumber<<"open";
        std::shared_ptr<G35Device>shPtrDevice=std::make_shared<G35Device>(device);
        shPtrDevice->setCallback(CallbackFactory::singleG35CallbackInstance(buffer));
        return static_cast<std::shared_ptr<IDevice>>(shPtrDevice);
    }
    else
    {
        printf("Failed to open device. Error code=%08X\n",GetLastError());
    }
    return nullptr;
}
//*************
ShPtrDevice DeviceSelector::coherentG35DeviceInstance(unsigned int deviceSetIndex)
{
    ICohG35DDCDeviceSetEnumerator *enumerator;
    ICohG35DDCDeviceSet *deviceSet;

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"Enumerator error";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET,
                       reinterpret_cast<void**>(&deviceSet))){
        qDebug()<<"DeviceSet error";
        return nullptr;
    }
    quint32 countDeviceInSet=0;
    G35DDC_DEVICE_INFO *deviceInfoMas=nullptr;

//    if(numberAvailableDeviceSet()>0)
//    {
//    enumerator->GetDeviceSetInfo(deviceSetIndex,nullptr,&countDeviceInSet);
//    deviceInfoMas=new G35DDC_DEVICE_INFO[countDeviceInSet];
//    enumerator->GetDeviceSetInfo(deviceSetIndex,deviceInfoMas,&countDeviceInSet);
//    }
//    else
//    {
        countDeviceInSet=1;
        deviceInfoMas=static_cast<G35DDC_DEVICE_INFO*>(G35DDC_OPEN_DEMO_SET);
//    }

    if(deviceSet->Open(deviceInfoMas,countDeviceInSet)){
        qDebug()<<"Device Set"<<deviceSetIndex<<"- count device:"<<countDeviceInSet;
        for(quint32 deviceIndex=0;deviceIndex<countDeviceInSet;deviceIndex++)
            qDebug()<<"|_____SerialNumber:" <<deviceInfoMas[deviceIndex].SerialNumber;

        delete []deviceInfoMas;
        enumerator->Release();
        //        return deviceSet;
        //        std::shared_ptr<CohG35DeviceSet> shPtr= std::make_shared<CohG35DeviceSet>(deviceSet);
        //        shPtr->setCallback(CallbackFactory::cohG35CallbackInstance(buffer));
        return std::make_shared<CohG35DeviceSet>(deviceSet);
    }
    qDebug()<<"Device is not open";
    delete [] deviceInfoMas;

    enumerator->Release();
    deviceSet->Release();
    return nullptr;
}

ShPtrDevice DeviceSelector::singleG35DeviceInstance(unsigned int deviceIndex)
{
    IG35DDCDevice *device=nullptr;
    IG35DDCDeviceEnumerator *enumerator=nullptr;
    G35DDC_DEVICE_INFO devInfo;

    if(!createInstance(G35DDC_CLASS_ID_DEVICE_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"FAILED ENUMERATOR INSTANCE";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_DEVICE,
                       reinterpret_cast<void**>(&device))){
        qDebug()<<"FAILED DEVICE INSTANCE";
        return nullptr;
    }

    enumerator->Enumerate();

    quint32 count=enumerator->GetCount();

    if(count!=0&&deviceIndex<count)
    {
        printf("Available G35DDC devices count=%d:\n",count);

        enumerator->GetDeviceInfo(deviceIndex,&devInfo,sizeof(devInfo));
        printf("%d. SN: %s\n",deviceIndex,devInfo.SerialNumber);
    }
    else
    {
        printf("No available G35DDC device found.\n");
        return nullptr;
    }

    enumerator->Release();

    //Opening the first available G35DDC device using predefined G3XDDC_OPEN_FIRST constant
    if(device->Open(devInfo.DevicePath))
    {
        qDebug()<<"Device:"<<deviceIndex<<devInfo.SerialNumber<<"open";
        return std::make_shared<G35Device>(device);
    }
    else
    {
        printf("Failed to open device. Error code=%08X\n",GetLastError());
    }
    return nullptr;
}

unsigned int DeviceSelector::numberAvailableDeviceSet(){
    ICohG35DDCDeviceSetEnumerator *enumerator;
    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
                       reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"Enumerator error";
        return 0;
    }
    unsigned int number=enumerator->GetDeviceSetCount();
    enumerator->Release();
    return number;
}
