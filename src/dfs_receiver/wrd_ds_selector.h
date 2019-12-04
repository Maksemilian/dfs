#ifndef DEVICE_SET_SELECTOR_H
#define DEVICE_SET_SELECTOR_H

#include "G35DDCAPI.h"
#include "wrd_coh_g35_ds.h"
#include <memory>

using ShPtrCohG35DeviceSet=std::shared_ptr<CohG35DeviceSet>;

class DeviceSetSelector
{
public:
    static std::shared_ptr<CohG35DeviceSet>selectDeviceSet(unsigned int deviceSetIndex);
    static unsigned int numberAvailableDeviceSet();
};

#endif // DEVICE_SET_SELECTOR_H
