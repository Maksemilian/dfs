#ifndef COH_G35_DS_SETTINGS_H
#define COH_G35_DS_SETTINGS_H

#include <QPair>
using Preselectors = std::pair<quint32, quint32>;

/*!
 * \addtogroup base
 */

///@{

/*!
 * \brief Структура содержит основные настройки устройства.
 */
struct DeviceSettings
{
    unsigned int attenuator;
    Preselectors preselectors;
    bool preamplifier;
    bool adcEnabled;
    unsigned short threshold;
    unsigned int frequency;
    unsigned int ddcType;
    unsigned int samplesPerBuffer;
    bool powerEnabled;
};

///@}

#endif // COH_G35_DS_SETTINGS_H
