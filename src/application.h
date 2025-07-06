#ifndef APPLICATION_H
#define APPLICATION_H

#include <array>
#include "config.h"
#include "hvac_data.h"
#include "logging/log_manager.h"
#include "config/config_manager.h"
#include "logic/alert_manager.h"
#include "logic/data_aggregator.h"
#include "data_processing.h"
#include "fs/SPIFFSFileSystem.h"
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

// A context structure to hold shared application data.
// This helps to decouple managers by passing a single context object
// instead of multiple individual references.
struct AppDataContext {
    HVACData& latestData;
    const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer;
    const size_t& bufferIndex;
    const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& aggregatedBuffer;
    const size_t& aggregatedBufferIndex;
};

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
    AppDataContext _appDataContext;
    HVACData _hvacData;
    SPIFFSFileSystem _spiffs; // The concrete filesystem object
    // Managers - order matters for initialization
    ConfigManager _configManager;
    LogManager _logManager;
    DataManager _dataManager;
    NetworkManager _networkManager;
    DisplayManager _displayManager;
    unsigned long _lastSensorReadTime;

    void performSensorReadCycle();
    void performAggregation();
    void logStatus();
    // Helper methods to make setup() more readable
    void setupSerial();
    void setupFileSystem();
    void setupHardware();
    void setupWatchdog();
};

#endif // APPLICATION_H