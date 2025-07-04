#include "dallas_temperature_adapter.h"

DallasTemperatureAdapter::DallasTemperatureAdapter(DallasTemperature& sensor)
    : _sensor(sensor) {}

void DallasTemperatureAdapter::requestTemperatures() {
    _sensor.requestTemperatures();
}

float DallasTemperatureAdapter::getTempC(const DeviceAddress& address) {
    float temp = _sensor.getTempC(address);
    return (temp == DEVICE_DISCONNECTED_C) ? -127.0f : temp;
}