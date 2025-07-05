#ifndef APPLICATION_H
#define APPLICATION_H

#include <array>
#include "config.h"
#include "hvac_data.h"
#include "config/config_manager.h"
#include "logic/alert_manager.h"
#include "logic/data_aggregator.h"
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
    std::array<HVACData, DATA_BUFFER_SIZE> _dataBuffer; // Primary buffer for raw data
    size_t _dataBufferIndex;                            // Index for the primary buffer
    std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE> _aggregatedDataBuffer;
    size_t _aggregatedDataBufferIndex;
    size_t _aggregationCycleCounter;
    HVACData _hvacData;
    ConfigManager _configManager;
    DataManager _dataManager;
    NetworkManager _networkManager;
    DisplayManager _displayManager;
    unsigned long _lastSensorReadTime;

    void performAggregation();
};

#endif // APPLICATION_H