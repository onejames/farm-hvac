#ifndef APPLICATION_H
#define APPLICATION_H

#include "hvac_data.h"
#include "data_processing.h"
#include "network/network_manager.h"
#include "display/display_manager.h"

// Hardware Libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>

// Adapter Libraries
#include "adapters/dallas_temperature_adapter.h"
#include "adapters/emon_lib_adapter.h"

// Network Libraries
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>

class Application {
public:
    Application();
    void setup();
    void loop();

private:
    // Hardware Objects
    OneWire _oneWire;
    DallasTemperature _tempSensors;
    EnergyMonitor _fanMonitor, _compressorMonitor, _pumpsMonitor;

    // Adapters
    DallasTemperatureAdapter _tempAdapter;
    EmonLibAdapter _fanAdapter, _compressorAdapter, _pumpsAdapter;

    // Core Logic and State
    HVACData _hvacData;
    DataManager _dataManager;
    NetworkManager _networkManager;
    DisplayManager _displayManager;
    unsigned long _lastSensorReadTime;
};

#endif // APPLICATION_H