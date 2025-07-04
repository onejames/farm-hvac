#include "data_processing.h"

DataManager::DataManager(ITemperatureSensor& tempSensors, ICurrentSensor& fan,
                         ICurrentSensor& compressor, ICurrentSensor& pumps,
                         const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr)
    : _tempSensors(tempSensors),
      _fanMonitor(fan),
      _compressorMonitor(compressor),
      _pumpsMonitor(pumps),
      _returnAddr(returnAddr),
      _supplyAddr(supplyAddr) {}

void DataManager::readAllSensors(HVACData& data, unsigned int adcSamples) {
  // Read Temperatures
  _tempSensors.requestTemperatures();
  data.returnTempC = _tempSensors.getTempC(_returnAddr);
  data.supplyTempC = _tempSensors.getTempC(_supplyAddr);

  // Read Currents
  data.fanAmps = _fanMonitor.calcIrms(adcSamples);
  data.compressorAmps = _compressorMonitor.calcIrms(adcSamples);
  data.geoPumpsAmps = _pumpsMonitor.calcIrms(adcSamples);
}

#ifdef ARDUINO
#include <Arduino.h> // For Serial.printf
void DataManager::printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, data.alertStatus.c_str());
}
#endif // ARDUINO

void DataManager::processSensorData(HVACData& data, float ampsOnThreshold) {
  // Calculate Delta T
  if (data.returnTempC != -127.0 && data.supplyTempC != -127.0) {
    data.deltaT = data.returnTempC - data.supplyTempC; // Assumes cooling mode
  } else {
    data.deltaT = 0;
  }

  // Determine Component Status based on amperage
  data.fanStatus = (data.fanAmps > ampsOnThreshold) ? "ON" : "OFF";
  data.compressorStatus = (data.compressorAmps > ampsOnThreshold) ? "ON" : "OFF";
  data.geoPumpsStatus = (data.geoPumpsAmps > ampsOnThreshold) ? "ON" : "OFF";

  // Determine Airflow Status (Placeholder for actual sensor)
  if (data.fanStatus == "ON") {
      data.airflowStatus = "OK"; // Assume OK if fan is running
  } else {
      data.airflowStatus = "N/A";
  }

  // Basic Anomaly Detection (PRD FR-4.1)
  if (data.fanStatus == "ON" && data.airflowStatus != "OK") {
      data.alertStatus = "ALERT: Fan ON, No Airflow";
  } else {
      data.alertStatus = "NONE";
  }
}