#include "device_set_selector.h"

#include <QDebug>

extern G3XDDCAPI_CREATE_INSTANCE createInstance;

ICohG35DDCDeviceSet *DeviceSetSelector::selectDeviceSet(unsigned int deviceSetIndex){
    ICohG35DDCDeviceSetEnumerator *enumerator;
    ICohG35DDCDeviceSet *deviceSet;

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,(void**)&enumerator)){
        qDebug()<<"Enumerator error";
        return nullptr;
    }

    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET,(void**)&deviceSet)){
        qDebug()<<"DeviceSet error";
        return nullptr;
    }

    quint32 countDeviceInSet=0;
    enumerator->GetDeviceSetInfo(deviceSetIndex,NULL,&countDeviceInSet);

    G35DDC_DEVICE_INFO *deviceInfoMas=new G35DDC_DEVICE_INFO[countDeviceInSet];
    enumerator->GetDeviceSetInfo(deviceSetIndex,deviceInfoMas,&countDeviceInSet);

    if(deviceSet->Open(deviceInfoMas,countDeviceInSet)){
        qDebug()<<"Device Set"<<deviceSetIndex<<"- count device:"<<countDeviceInSet;
        for(quint32 deviceIndex=0;deviceIndex<countDeviceInSet;deviceIndex++)
            qDebug()<<"|_____SerialNumber:" <<deviceInfoMas[deviceIndex].SerialNumber;

        delete []deviceInfoMas;
        enumerator->Release();
        return deviceSet;
    }
    qDebug()<<"Device is not open";
    delete [] deviceInfoMas;

    enumerator->Release();
    deviceSet->Release();
    return nullptr;
}

unsigned DeviceSetSelector::numberAvailableDeviceSet(){
    ICohG35DDCDeviceSetEnumerator *enumerator;
    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,(void**)&enumerator)){
        qDebug()<<"Enumerator error";
        return 0;
    }
    unsigned int number=enumerator->GetDeviceSetCount();
    enumerator->Release();
    return number;
}