#include "hardware_manager.h"

#ifdef ARDUINO
HardwareManager::HardwareManager()
    // Initialize hardware objects
    : _oneWire(ONE_WIRE_BUS_PIN),
      _tempSensors(&_oneWire),
    // Initialize adapters, passing references to the hardware objects
      _tempAdapter(_tempSensors),
      _fanAdapter(_fanMonitor),
      _compressorAdapter(_compressorMonitor),
      _pumpsAdapter(_pumpsMonitor)
{}

void HardwareManager::setup() {
    _tempSensors.begin();
    _fanMonitor.current(FAN_CT_PIN, CT_CALIBRATION);
    _compressorMonitor.current(COMPRESSOR_CT_PIN, CT_CALIBRATION);
    _pumpsMonitor.current(PUMPS_CT_PIN, CT_CALIBRATION);
}
#else
// Native build "hollow" implementations
HardwareManager::HardwareManager()
    // Adapters are default-initialized using their native constructors
{}

void HardwareManager::setup() {}
#endif

ITemperatureSensor& HardwareManager::getTempAdapter() {
    return _tempAdapter;
}

ICurrentSensor& HardwareManager::getFanAdapter() {
    return _fanAdapter;
}

ICurrentSensor& HardwareManager::getCompressorAdapter() {
    return _compressorAdapter;
}

ICurrentSensor& HardwareManager::getPumpsAdapter() {
    return _pumpsAdapter;
}
