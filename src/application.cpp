#include "application.h"
#include "config.h"
#include <SPIFFS.h>
#include "secrets.h"
#include "version.h"
#include "esp_task_wdt.h"

Application::Application()
    : _systemState(),
      _aggregationCycleCounter(0),
      _hardwareManager(),
      _spiffs(),
      _configManager(_spiffs),
      _logManager(_spiffs),
      _dataManager(_hardwareManager.getTempAdapter(), _hardwareManager.getFanAdapter(), _hardwareManager.getCompressorAdapter(), _hardwareManager.getPumpsAdapter(), returnAirSensorAddress, supplyAirSensorAddress),
      _networkManager(_systemState, _configManager, _logManager),
      _displayManager(),
      _lastSensorReadTime(0) {}

void Application::setup() {
    setupSerial();
    setupFileSystem();

    // Load configuration from SPIFFS
    _configManager.load();

    setupHardware();

    // Setup network and web interface
    _networkManager.setup();
    _logManager.log("Network setup complete. IP: %s", WiFi.localIP().toString().c_str());
    
    setupWatchdog();

    // Setup display
    if (!_displayManager.setup()) {
        _logManager.log("ERROR: SSD1306 allocation failed");
    }

    _logManager.log("Setup complete. Entering main loop.");
}

void Application::loop() {
    // Feed the watchdog timer on every loop to show that the system is responsive.
    esp_task_wdt_reset();

    // Handle non-blocking network tasks on every loop
    _networkManager.handleClient();

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
    Serial.printf("[App] Performed data aggregation cycle. Avg dT: %.2f\n", aggregatedData.avgDeltaT);

    _networkManager.publishAggregatedData();
}

void Application::setupSerial() {
    Serial.begin(115200);
    while (!Serial);
}

void Application::setupFileSystem() {
    if(!_spiffs.begin()){
        Serial.println("CRITICAL ERROR: An Error has occurred while mounting SPIFFS. Halting.");
        while(1) { delay(1000); } // Halt execution
    }
    _logManager.begin(); // This is now empty but kept for consistency.
    _logManager.log("System boot. Version: %s", FIRMWARE_VERSION);
}

void Application::setupHardware() {
    _hardwareManager.setup();
}

void Application::setupWatchdog() {
    // Initialize the watchdog timer. If the main loop freezes for more than
    // WATCHDOG_TIMEOUT_S, the ESP32 will automatically reboot.
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true); // true = panic on timeout
    esp_task_wdt_add(NULL); // Add current task (main loop) to watchdog
}