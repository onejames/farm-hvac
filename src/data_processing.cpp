#include <Arduino.h>
#include "data_processing.h"

#include "config.h"

DataManager::DataManager(DallasTemperature& tempSensors, EnergyMonitor& fan,
                         EnergyMonitor& compressor, EnergyMonitor& pumps,
                         const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr)
    : _tempSensors(tempSensors),
      _fanMonitor(fan),
      _compressorMonitor(compressor),
      _pumpsMonitor(pumps),
      _returnAddr(returnAddr),
      _supplyAddr(supplyAddr) {}

void DataManager::readAllSensors(HVACData& data) {
  // Read Temperatures
  _tempSensors.requestTemperatures();
  data.returnTempC = _tempSensors.getTempC(_returnAddr);
  data.supplyTempC = _tempSensors.getTempC(_supplyAddr);

  // Handle sensor read errors
  if (data.returnTempC == DEVICE_DISCONNECTED_C) {
    data.returnTempC = -127.0; // Use a known error value
    Serial.println("[ERROR] Failed to read return air temperature sensor.");
  }
  if (data.supplyTempC == DEVICE_DISCONNECTED_C) {
    data.supplyTempC = -127.0; // Use a known error value
    Serial.println("[ERROR] Failed to read supply air temperature sensor.");
  }

  // Read Currents
  data.fanAmps = _fanMonitor.calcIrms(ADC_SAMPLES_COUNT);
  data.compressorAmps = _compressorMonitor.calcIrms(ADC_SAMPLES_COUNT);
  data.geoPumpsAmps = _pumpsMonitor.calcIrms(ADC_SAMPLES_COUNT);
}

void DataManager::processSensorData(HVACData& data) {
  // Calculate Delta T
  if (data.returnTempC != -127.0 && data.supplyTempC != -127.0) {
    data.deltaT = data.returnTempC - data.supplyTempC; // Assumes cooling mode
  } else {
    data.deltaT = 0;
  }

  // Determine Component Status based on amperage
  data.fanStatus = (data.fanAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";
  data.compressorStatus = (data.compressorAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";
  data.geoPumpsStatus = (data.geoPumpsAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";

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

void DataManager::printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, data.alertStatus.c_str());
}