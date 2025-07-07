#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "IHardwareManager.h"
// Use conditional compilation to include hardware-specific libraries only for Arduino builds.
#ifdef ARDUINO
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>
#endif

#include "interfaces/i_current_sensor.h"
#include "adapters/dallas_temperature_adapter.h"
#include "adapters/emon_lib_adapter.h"
#include "config.h"

class HardwareManager : public IHardwareManager {
public:
    HardwareManager();

    void setup() override;

    // Public accessors for adapters so they can be injected into other managers
    [[nodiscard]] ITemperatureSensor& getTempAdapter() override;
    [[nodiscard]] ICurrentSensor& getFanAdapter() override;
    [[nodiscard]] ICurrentSensor& getCompressorAdapter() override;
    [[nodiscard]] ICurrentSensor& getPumpsAdapter() override;

private:
#ifdef ARDUINO
    // Hardware Objects
    OneWire _oneWire;
    DallasTemperature _tempSensors;
    EnergyMonitor _fanMonitor, _compressorMonitor, _pumpsMonitor;
#endif

    // Adapters
    DallasTemperatureAdapter _tempAdapter;
    EmonLibAdapter _fanAdapter, _compressorAdapter, _pumpsAdapter;
};

#endif // HARDWARE_MANAGER_H
