#include "wrd_coh_g35_ds_selector.h"
#include "wrd_coh_g35_callback.h"
#include <QDebug>

extern G3XDDCAPI_CREATE_INSTANCE createInstance;

ShPtrCohG35DeviceSet DeviceSetSelector::selectDeviceSet(unsigned int deviceSetIndex,
                                                        const std::shared_ptr<RingBuffer<proto::receiver::Packet>>&buffer)
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
        return shPtr;
    }
    qDebug()<<"Device is not open";
    delete [] deviceInfoMas;

    enumerator->Release();
    deviceSet->Release();
    return nullptr;
}

//ICohG35DDCDeviceSet *DeviceSetSelector::selectDeviceSet(unsigned int deviceSetIndex){
//    ICohG35DDCDeviceSetEnumerator *enumerator;
//    ICohG35DDCDeviceSet *deviceSet;

//    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,
//                       reinterpret_cast<void**>(&enumerator))){
//        qDebug()<<"Enumerator error";
//        return nullptr;
//    }

//    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET,
//                       reinterpret_cast<void**>(&deviceSet))){
//        qDebug()<<"DeviceSet error";
//        return nullptr;
//    }

//    quint32 countDeviceInSet=0;
//    enumerator->GetDeviceSetInfo(deviceSetIndex,nullptr,&countDeviceInSet);

//    G35DDC_DEVICE_INFO *deviceInfoMas=new G35DDC_DEVICE_INFO[countDeviceInSet];
//    enumerator->GetDeviceSetInfo(deviceSetIndex,deviceInfoMas,&countDeviceInSet);

//    if(deviceSet->Open(deviceInfoMas,countDeviceInSet)){
//        qDebug()<<"Device Set"<<deviceSetIndex<<"- count device:"<<countDeviceInSet;
//        for(quint32 deviceIndex=0;deviceIndex<countDeviceInSet;deviceIndex++)
//            qDebug()<<"|_____SerialNumber:" <<deviceInfoMas[deviceIndex].SerialNumber;

//        delete []deviceInfoMas;
//        enumerator->Release();
//        return deviceSet;
//    }
//    qDebug()<<"Device is not open";
//    delete [] deviceInfoMas;

//    enumerator->Release();
//    deviceSet->Release();
//    return nullptr;
//}

unsigned int DeviceSetSelector::numberAvailableDeviceSet(){
    ICohG35DDCDeviceSetEnumerator *enumerator;
    if(!createInstance(G35DDC_CLASS_ID_COH_DEVICE_SET_ENUMERATOR,reinterpret_cast<void**>(&enumerator))){
        qDebug()<<"Enumerator error";
        return 0;
    }
    unsigned int number=enumerator->GetDeviceSetCount();
    enumerator->Release();
    return number;
}