#ifndef DALLAS_TEMPERATURE_ADAPTER_H
#define DALLAS_TEMPERATURE_ADAPTER_H

#include "interfaces/i_temperature_sensor.h"
#include <DallasTemperature.h>

class DallasTemperatureAdapter : public ITemperatureSensor {
public:
    DallasTemperatureAdapter(DallasTemperature& sensor);
    void requestTemperatures() override;
    float getTempC(const DeviceAddress& address) override;
private:
    DallasTemperature& _sensor;
};

#endif // DALLAS_TEMPERATURE_ADAPTER_H