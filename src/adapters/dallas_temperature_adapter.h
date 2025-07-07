#ifndef DALLAS_TEMPERATURE_ADAPTER_H
#define DALLAS_TEMPERATURE_ADAPTER_H

#include "interfaces/i_temperature_sensor.h"

// Use conditional compilation to include hardware-specific libraries only for Arduino builds.
#ifdef ARDUINO
#include <DallasTemperature.h>
#endif
#include "hvac_hardware_types.h"

class DallasTemperatureAdapter : public ITemperatureSensor {
public:
#ifdef ARDUINO
    explicit DallasTemperatureAdapter(DallasTemperature& sensors);
#else
    DallasTemperatureAdapter(); // Default constructor for native builds
#endif
    void requestTemperatures() override;
    float getTempC(const DeviceAddress& address) override;

private:
#ifdef ARDUINO
    DallasTemperature& _sensors;
#endif
};

#endif // DALLAS_TEMPERATURE_ADAPTER_H