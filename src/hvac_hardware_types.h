#ifndef HVAC_HARDWARE_TYPES_H
#define HVAC_HARDWARE_TYPES_H

#ifdef ARDUINO
// On the ESP32, DeviceAddress is defined in the DallasTemperature library.
#include <DallasTemperature.h>
#else
// For native testing, we must define the type ourselves.
#include <cstdint>
typedef uint8_t DeviceAddress[8];
#endif

#endif // HVAC_HARDWARE_TYPES_H