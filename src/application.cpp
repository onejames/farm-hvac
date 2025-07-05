#include "application.h"
#include "config.h"
#include "secrets.h"
#include "version.h"

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
      _networkManager(_hvacData),
      _displayManager() {
          _lastSensorReadTime = 0;
      }

void Application::setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("Booting HVAC Monitor...");
    Serial.printf("Version: %s, Built: %s\n", FIRMWARE_VERSION, BUILD_DATE);

    // Initialize hardware sensors
    _tempSensors.begin();
    _fanMonitor.current(FAN_CT_PIN, CT_CALIBRATION);
    _compressorMonitor.current(COMPRESSOR_CT_PIN, CT_CALIBRATION);
    _pumpsMonitor.current(PUMPS_CT_PIN, CT_CALIBRATION);

    // Setup network and web interface
    _networkManager.setup();
    
    // Setup display
    _displayManager.setup();

    Serial.println("Setup complete. Entering main loop.");
}

void Application::loop() {
    // Handle non-blocking network tasks on every loop
    _networkManager.handleClient();

    // The main sensor read and publish cycle is throttled
    unsigned long currentTime = millis();
    if (currentTime - _lastSensorReadTime >= SENSOR_READ_INTERVAL_MS) {
        _lastSensorReadTime = currentTime;

        _dataManager.readAndProcessData(_hvacData, ADC_SAMPLES, AMPS_ON_THRESHOLD);
        _networkManager.publish();
        _dataManager.printStatus(_hvacData);
    }

    // The display can update on its own, more frequent schedule
    _displayManager.update(_hvacData);
}