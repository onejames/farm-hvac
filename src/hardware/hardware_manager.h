#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>
#include "interfaces/i_current_sensor.h"
#include "adapters/dallas_temperature_adapter.h"
#include "adapters/emon_lib_adapter.h"
#include "config.h"

class HardwareManager {
public:
    HardwareManager();

    void setup();

    // Public accessors for adapters so they can be injected into other managers
    [[nodiscard]] ITemperatureSensor& getTempAdapter();
    [[nodiscard]] ICurrentSensor& getFanAdapter();
    [[nodiscard]] ICurrentSensor& getCompressorAdapter();
    [[nodiscard]] ICurrentSensor& getPumpsAdapter();

private:
    // Hardware Objects
    OneWire _oneWire;
    DallasTemperature _tempSensors;
    EnergyMonitor _fanMonitor, _compressorMonitor, _pumpsMonitor;

    // Adapters
    DallasTemperatureAdapter _tempAdapter;
    EmonLibAdapter _fanAdapter, _compressorAdapter, _pumpsAdapter;
};

#endif // HARDWARE_MANAGER_H
