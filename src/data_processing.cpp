#include <Arduino.h>
#include "data_processing.h"

#include "config.h"

// Note: This module no longer uses 'extern' to access global sensor objects.
// Instead, they are passed into the functions (dependency injection).
// This makes the module more self-contained and easier to test.

// --- Function Implementations ---

void readTemperatureSensors(HVACData& data, DallasTemperature& tempSensors,
                            const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr) {
  tempSensors.requestTemperatures();
  data.returnTempC = tempSensors.getTempC(returnAddr);
  data.supplyTempC = tempSensors.getTempC(supplyAddr);

  // Handle sensor read errors
  if (data.returnTempC == DEVICE_DISCONNECTED_C) {
    data.returnTempC = -127.0; // Use a known error value
    Serial.println("[ERROR] Failed to read return air temperature sensor.");
  }
  if (data.supplyTempC == DEVICE_DISCONNECTED_C) {
    data.supplyTempC = -127.0; // Use a known error value
    Serial.println("[ERROR] Failed to read supply air temperature sensor.");
  }
}

void readCurrentSensors(HVACData& data, EnergyMonitor& fan,
                        EnergyMonitor& compressor, EnergyMonitor& pumps) {
  // EmonLib calculates RMS current over a number of samples.
  data.fanAmps = fan.calcIrms(ADC_SAMPLES_COUNT);
  data.compressorAmps = compressor.calcIrms(ADC_SAMPLES_COUNT);
  data.geoPumpsAmps = pumps.calcIrms(ADC_SAMPLES_COUNT);
}

void processSensorData(HVACData& data) {
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
  // This is a simplified logic based on the fan's status.
  // A real implementation would read an airflow sensor (PRD FR-1.2) and might
  // return "OK" or "NO FLOW".
  if (data.fanStatus == "ON") {
      data.airflowStatus = "OK"; // Assume OK if fan is running
  } else {
      data.airflowStatus = "N/A";
  }

  // Basic Anomaly Detection (PRD FR-4.1)
  // This is a simplified check. A full implementation would require tracking state over time.
  if (data.fanStatus == "ON" && data.airflowStatus != "OK") {
      data.alertStatus = "ALERT: Fan ON, No Airflow";
  } else {
      data.alertStatus = "NONE";
  }
}

void printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, data.alertStatus.c_str());
}