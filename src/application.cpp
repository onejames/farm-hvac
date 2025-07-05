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
      _networkManager(_hvacData, _dataBuffer, _dataBufferIndex, _aggregatedDataBuffer, _aggregatedDataBufferIndex),
      _displayManager(), _dataBufferIndex(0), _aggregatedDataBufferIndex(0),
      _aggregationCycleCounter(0), _lastSensorReadTime(0) {}

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

        // Read sensor data and process it into the _hvacData member.
        _dataManager.readAndProcessData(_hvacData, ADC_SAMPLES, AMPS_ON_THRESHOLD);

        // Store the latest measurement in our historical data buffer.
        _dataBuffer[_dataBufferIndex] = _hvacData;
        _dataBufferIndex = (_dataBufferIndex + 1) % DATA_BUFFER_SIZE;

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
    double sumReturnTemp = 0.0;
    double sumSupplyTemp = 0.0;
    double sumDeltaT = 0.0;
    double sumFanAmps = 0.0;
    double sumCompressorAmps = 0.0;
    double sumGeoPumpsAmps = 0.0;
    size_t validSamples = 0;

    for (const auto& data : _dataBuffer) {
        // Skip uninitialized entries in the buffer
        if (data.returnTempC == -127.0f && data.fanAmps == 0.0) {
            continue;
        }
        validSamples++;
        sumReturnTemp += data.returnTempC;
        sumSupplyTemp += data.supplyTempC;
        sumDeltaT += data.deltaT;
        sumFanAmps += data.fanAmps;
        sumCompressorAmps += data.compressorAmps;
        sumGeoPumpsAmps += data.geoPumpsAmps;
    }

    if (validSamples == 0) return; // Nothing to aggregate

    AggregatedHVACData aggregatedData;
    aggregatedData.timestamp = millis();
    aggregatedData.avgReturnTempC = sumReturnTemp / validSamples;
    aggregatedData.avgSupplyTempC = sumSupplyTemp / validSamples;
    aggregatedData.avgDeltaT = sumDeltaT / validSamples;
    aggregatedData.avgFanAmps = sumFanAmps / validSamples;
    aggregatedData.avgCompressorAmps = sumCompressorAmps / validSamples;
    aggregatedData.avgGeoPumpsAmps = sumGeoPumpsAmps / validSamples;
    aggregatedData.lastFanStatus = _hvacData.fanStatus;
    aggregatedData.lastCompressorStatus = _hvacData.compressorStatus;
    aggregatedData.lastGeoPumpsStatus = _hvacData.geoPumpsStatus;

    _aggregatedDataBuffer[_aggregatedDataBufferIndex] = aggregatedData;
    _aggregatedDataBufferIndex = (_aggregatedDataBufferIndex + 1) % AGGREGATED_DATA_BUFFER_SIZE;
    Serial.printf("[App] Performed data aggregation cycle. Avg dT: %.2f\n", aggregatedData.avgDeltaT);

    _networkManager.publishAggregatedData();
}