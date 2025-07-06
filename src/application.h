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
#include "state/SystemState.h"
#include "hardware/hardware_manager.h"
#include "fs/SPIFFSFileSystem.h"
#include "network/network_manager.h"
#include "display/display_manager.h"

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
    SystemState _systemState;
    size_t _aggregationCycleCounter;
    HardwareManager _hardwareManager;
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