#ifndef I_TEMPERATURE_SENSOR_H
#define I_TEMPERATURE_SENSOR_H

#include "hvac_hardware_types.h" // For DeviceAddress

class ITemperatureSensor {
public:
    virtual void requestTemperatures() = 0;
    virtual float getTempC(const DeviceAddress& address) = 0;
};
#endif // I_TEMPERATURE_SENSOR_H