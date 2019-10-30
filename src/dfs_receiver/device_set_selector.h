#ifndef DEVICE_SET_SELECTOR_H
#define DEVICE_SET_SELECTOR_H

#include "G35DDCAPI.h"

class DeviceSetSelector
{
public:
    static ICohG35DDCDeviceSet* selectDeviceSet(unsigned int deviceSetIndex);
    static unsigned int numberAvailableDeviceSet();
};

#endif // DEVICE_SET_SELECTOR_H
