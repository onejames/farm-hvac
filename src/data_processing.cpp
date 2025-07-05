#include "data_processing.h"
#include "logic/enum_converters.h" // For toString helpers
#include <cmath> // For isnan

DataManager::DataManager(ITemperatureSensor& tempSensors, ICurrentSensor& fan,
                         ICurrentSensor& compressor, ICurrentSensor& pumps,
                         const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr)
    : _tempSensors(tempSensors),
      _fanMonitor(fan),
      _compressorMonitor(compressor),
      _pumpsMonitor(pumps),
      _returnAddr(returnAddr),
      _supplyAddr(supplyAddr) {}

void DataManager::readAndProcessData(HVACData& data, unsigned int adcSamples, float ampsThreshold) {
  // 1. Read raw sensor data
  _tempSensors.requestTemperatures();
  data.returnTempC = _tempSensors.getTempC(_returnAddr);
  data.supplyTempC = _tempSensors.getTempC(_supplyAddr);

  data.fanAmps = _fanMonitor.calcIrms(adcSamples);
  data.compressorAmps = _compressorMonitor.calcIrms(adcSamples);
  data.geoPumpsAmps = _pumpsMonitor.calcIrms(adcSamples);

  // 2. Process the raw data
  // Calculate Delta T, handling sensor errors
  if (data.returnTempC != -127.0f && data.supplyTempC != -127.0f) {
      data.deltaT = data.returnTempC - data.supplyTempC;
  } else {
      data.deltaT = 0.0f;
  }

  // Determine component status based on amperage
  if (std::isnan(data.fanAmps)) {
      data.fanStatus = ComponentStatus::UNKNOWN;
  } else {
      data.fanStatus = (data.fanAmps > ampsThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;
  }

  if (std::isnan(data.compressorAmps)) {
      data.compressorStatus = ComponentStatus::UNKNOWN;
  } else {
      data.compressorStatus = (data.compressorAmps > ampsThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;
  }

  if (std::isnan(data.geoPumpsAmps)) {
      data.geoPumpsStatus = ComponentStatus::UNKNOWN;
  } else {
      data.geoPumpsStatus = (data.geoPumpsAmps > ampsThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;
  }

  // Basic airflow check and alert status
  // This is a placeholder. A real implementation would check an airflow sensor.
  data.airflowStatus = (data.fanStatus == ComponentStatus::ON) ? AirflowStatus::OK : AirflowStatus::NA;
  // For now, alerts are not implemented beyond this basic check.
  data.alertStatus = AlertStatus::NONE;
}

#ifdef ARDUINO
#include <Arduino.h> // For Serial.printf
void DataManager::printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, toString(data.alertStatus));
}
#endif // ARDUINO