#include "dallas_temperature_adapter.h"

#ifdef ARDUINO
// This constructor and methods are only for the Arduino build
DallasTemperatureAdapter::DallasTemperatureAdapter(DallasTemperature& sensors) : _sensors(sensors) {}

void DallasTemperatureAdapter::requestTemperatures() {
    _sensors.requestTemperatures();
}

float DallasTemperatureAdapter::getTempC(const DeviceAddress& address) {
    float temp = _sensors.getTempC(address);
    // The library returns a specific value for disconnected sensors.
    // We normalize this to our own standard for consistency.
    return (temp == DEVICE_DISCONNECTED_C) ? -127.0f : temp;
}

#else
// These are the "hollow" implementations for the native build environment.

DallasTemperatureAdapter::DallasTemperatureAdapter() {
    // In a native build, the _sensors reference doesn't exist, so this is empty.
}

void DallasTemperatureAdapter::requestTemperatures() { /* Do nothing in native build. */ }

float DallasTemperatureAdapter::getTempC(const DeviceAddress& /*address*/) {
    // For native tests, return a sensible default or mock value.
    return 20.0; // e.g., return a default room temperature
}
#endif