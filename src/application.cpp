#include "application.h"
#include <memory> // For std::make_unique
#include "config.h"
#include "network/PubSubClientWrapper.h"
#include "secrets.h"
#include "version.h"

#ifdef ARDUINO
#include <SPIFFS.h>
#include "esp_task_wdt.h"
#else
// Provide mock implementations for Arduino-specific functions like millis() for native tests
#include "mocks/Arduino.h"
#endif

#ifdef ARDUINO
Application::Application() // Full constructor for hardware builds
    : _systemState(),
      _aggregationCycleCounter(0),
      _net(),
      _mqttClient(_net),
      _hardwareManager(),
      _spiffs(),
      _configManager(_spiffs),
      _logManager(_spiffs),
      _dataManager(_hardwareManager, returnAirSensorAddress, supplyAirSensorAddress),
      _webServerManager(_systemState, _configManager, _logManager),
      _mqttManager(_systemState, _logManager, std::unique_ptr<PubSubClientWrapper>(new PubSubClientWrapper(_mqttClient))),
      _displayManager(),
      _lastSensorReadTime(0) {}
#else
Application::Application() // "Hollow" constructor for native testing
    : _systemState(),
      _aggregationCycleCounter(0),
      // _net and _mqttClient do not exist in native builds
      _hardwareManager(),
      _spiffs(),
      _configManager(_spiffs),
      _logManager(_spiffs),
      _dataManager(_hardwareManager, {}, {}), // Pass empty device addresses
      _webServerManager(_systemState, _configManager, _logManager),
      _mqttManager(_systemState, _logManager, nullptr), // Pass nullptr for the client
      _displayManager(),
      _lastSensorReadTime(0) {}
#endif

void Application::setup() {
#ifdef ARDUINO
    setupSerial();
    setupFileSystem();

    // Load configuration from SPIFFS
    _configManager.load();

    setupHardware();
    
    // Setup WiFi, WebServer, and MQTT Client
    setupNetwork();
    // Configure MQTT client before setting up the manager that uses it
    _net.setCACert(AWS_CERT_CA);
    _net.setCertificate(AWS_CERT_CRT);
    _net.setPrivateKey(AWS_CERT_PRIVATE);
    _mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
    _webServerManager.setup();
    _logManager.log("Network setup complete. IP: %s", WiFi.localIP().toString().c_str()); // WiFi is guarded in WebServerManager
    
    setupWatchdog();

    // Setup display
    if (!_displayManager.setup()) {
        _logManager.log("ERROR: SSD1306 allocation failed");
    }

    _logManager.log("Setup complete. Entering main loop.");
#endif
}

void Application::loop() {
#ifdef ARDUINO
    // Feed the watchdog timer on every loop to show that the system is responsive.
    esp_task_wdt_reset();
#endif

    // Handle non-blocking network tasks on every loop
    _mqttManager.handleClient();

    // The main sensor read and publish cycle is throttled
    unsigned long currentTime = millis();
    if (currentTime - _lastSensorReadTime >= SENSOR_READ_INTERVAL_MS) {
        _lastSensorReadTime = currentTime;
        performSensorReadCycle();
    }

    // The display can update on its own, more frequent schedule
    _displayManager.update(_systemState.getLatestData());
}

void Application::performSensorReadCycle() {
    // Read sensor data and process it into the _hvacData member.
    _dataManager.readAndProcessData(_systemState.getLatestData(), ADC_SAMPLES, AMPS_ON_THRESHOLD);

    // Store the latest measurement in our historical data buffer.
    _systemState.recordLatestData();

    // Check for alert conditions based on the historical data
    _systemState.getLatestData().alertStatus = AlertManager::checkAlerts(_systemState.getDataBuffer(), _configManager.getConfig());

    // Check if it's time to perform an aggregation cycle.
    _aggregationCycleCounter++;
    if (_aggregationCycleCounter >= DATA_BUFFER_SIZE) {
        performAggregation();
        _aggregationCycleCounter = 0;
    }

    // Log the current status to the serial monitor for debugging.
    logStatus();
}

void Application::logStatus() {
#ifdef ARDUINO
    // This provides a concise summary of the system state to the serial monitor.
    const HVACData& data = _systemState.getLatestData();
    Serial.printf("[Status] Ret: %.2fC, Sup: %.2fC, dT: %.2fC | Fan: %.2fA, Comp: %.2fA, Pumps: %.2fA | Alert: %d\n",
                  data.returnTempC,
                  data.supplyTempC,
                  data.deltaT,
                  data.fanAmps,
                  data.compressorAmps,
                  data.geoPumpsAmps,
                  static_cast<int>(data.alertStatus));
#endif
}

void Application::performAggregation() {
    // Use the dedicated aggregator to calculate the averages and capture final state
    AggregatedHVACData aggregatedData = DataAggregator::aggregate(_systemState.getDataBuffer(), _systemState.getLatestData());
    aggregatedData.timestamp = millis();

    _systemState.addAggregatedData(aggregatedData);
#ifdef ARDUINO
    Serial.printf("[App] Performed data aggregation cycle. Avg dT: %.2f\n", aggregatedData.avgDeltaT);
#endif

    _mqttManager.publishAggregatedData();
}

void Application::setupSerial() {
#ifdef ARDUINO
    Serial.begin(115200);
    while (!Serial);
#endif
}

void Application::setupFileSystem() {
#ifdef ARDUINO
    if(!_spiffs.begin()){
        Serial.println("CRITICAL ERROR: An Error has occurred while mounting SPIFFS. Halting.");
        while(1) { delay(1000); } // Halt execution
    }
#endif
    _logManager.begin(); // This is now empty but kept for consistency.
    _logManager.log("System boot. Version: %s", FIRMWARE_VERSION);
}

void Application::setupNetwork() {
#ifdef ARDUINO
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _logManager.log("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
#endif
}

void Application::setupHardware() {
    _hardwareManager.setup();
}

void Application::setupWatchdog() {
#ifdef ARDUINO
    // Initialize the watchdog timer. If the main loop freezes for more than
    // WATCHDOG_TIMEOUT_S, the ESP32 will automatically reboot.
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true); // true = panic on timeout
    esp_task_wdt_add(NULL); // Add current task (main loop) to watchdog
#endif
}