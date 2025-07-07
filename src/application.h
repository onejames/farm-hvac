#ifndef APPLICATION_H
#define APPLICATION_H

#include <array>
#include "config.h"
#include "hvac_data.h"
#include "logging/log_manager.h"
#include "config/config_manager.h"
#include "logic/alert_manager.h"
#include "logic/data_aggregator.h"
#include "DataManager.h"
#include "state/SystemState.h"
#include "hardware/hardware_manager.h"
#include "fs/SPIFFSFileSystem.h"
#include "network/WebServerManager.h" // Corrected path
#include "network/MqttManager.h"
#include "display/display_manager.h"

// Network Libraries - only include for Arduino builds
#ifdef ARDUINO
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>

#endif

class Application {
public:
    Application();
    void setup();
    void loop();

private:
    SystemState _systemState;
    size_t _aggregationCycleCounter;
    // Network objects are now owned by Application
#ifdef ARDUINO
    // Hardware-specific network objects are owned by Application
    WiFiClientSecure _net;
    PubSubClient _mqttClient;
#endif
    HardwareManager _hardwareManager;
    SPIFFSFileSystem _spiffs; // The concrete filesystem object
    // Managers - order matters for initialization
    ConfigManager _configManager;
    LogManager _logManager;
    DataManager _dataManager;
    WebServerManager _webServerManager;
    MqttManager _mqttManager;
    DisplayManager _displayManager;
    unsigned long _lastSensorReadTime;

    void performSensorReadCycle();
    void performAggregation();
    void logStatus();
    // Helper methods to make setup() more readable
    void setupSerial();
    void setupFileSystem();
    void setupNetwork();
    void setupHardware();
    void setupWatchdog();
};

#endif // APPLICATION_H