#include "application.h"
#include "config.h"
#include "secrets.h"
#include "version.h"
#include "esp_task_wdt.h"

Application::Application()
    // Initialize hardware objects
    : _oneWire(ONE_WIRE_BUS_PIN),
      _tempSensors(&_oneWire),
    // Initialize adapters, passing references to the hardware objects
      _tempAdapter(_tempSensors),
      _fanAdapter(_fanMonitor),
      _compressorAdapter(_compressorMonitor),
      _pumpsAdapter(_pumpsMonitor),
    // Initialize logic and network managers
      _dataManager(_tempAdapter, _fanAdapter, _compressorAdapter, _pumpsAdapter, returnAirSensorAddress, supplyAirSensorAddress),
      _networkManager(_hvacData, _dataBuffer, _dataBufferIndex, _aggregatedDataBuffer, _aggregatedDataBufferIndex, _configManager),
      _displayManager(), _configManager(), _dataBufferIndex(0),
      _aggregatedDataBufferIndex(0), _aggregationCycleCounter(0),
      _lastSensorReadTime(0) {}

void Application::setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Booting HVAC Monitor...");
    Serial.printf("Version: %s, Built: %s\n", FIRMWARE_VERSION, BUILD_DATE);

    // Load configuration from SPIFFS
    _configManager.load();

    // Initialize hardware sensors
    _tempSensors.begin();
    _fanMonitor.current(FAN_CT_PIN, CT_CALIBRATION);
    _compressorMonitor.current(COMPRESSOR_CT_PIN, CT_CALIBRATION);
    _pumpsMonitor.current(PUMPS_CT_PIN, CT_CALIBRATION);

    // Setup network and web interface
    _networkManager.setup();
    
    // Initialize the watchdog timer. If the main loop freezes for more than
    // WATCHDOG_TIMEOUT_S, the ESP32 will automatically reboot.
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true); // true = panic on timeout
    esp_task_wdt_add(NULL); // Add current task (main loop) to watchdog

    // Setup display
    _displayManager.setup();

    Serial.println("Setup complete. Entering main loop.");
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

        // Read sensor data and process it into the _hvacData member.
        _dataManager.readAndProcessData(_hvacData, ADC_SAMPLES, AMPS_ON_THRESHOLD);

        // Store the latest measurement in our historical data buffer.
        _dataBuffer[_dataBufferIndex] = _hvacData;
        _dataBufferIndex = (_dataBufferIndex + 1) % DATA_BUFFER_SIZE;

        // Check for alert conditions based on the historical data
        _hvacData.alertStatus = AlertManager::checkAlerts(_dataBuffer, _configManager.getConfig());

        // Check if it's time to perform an aggregation cycle.
        _aggregationCycleCounter++;
        if (_aggregationCycleCounter >= DATA_BUFFER_SIZE) {
            performAggregation();
            _aggregationCycleCounter = 0;
        }

        _dataManager.printStatus(_hvacData);
    }

    // The display can update on its own, more frequent schedule
    _displayManager.update(_hvacData);
}

void Application::performAggregation() {
    // Use the dedicated aggregator to calculate the averages and capture final state
    AggregatedHVACData aggregatedData = DataAggregator::aggregate(_dataBuffer, _hvacData);
    aggregatedData.timestamp = millis();

    _aggregatedDataBuffer[_aggregatedDataBufferIndex] = aggregatedData;
    _aggregatedDataBufferIndex = (_aggregatedDataBufferIndex + 1) % AGGREGATED_DATA_BUFFER_SIZE;
    Serial.printf("[App] Performed data aggregation cycle. Avg dT: %.2f\n", aggregatedData.avgDeltaT);

    _networkManager.publishAggregatedData();
}