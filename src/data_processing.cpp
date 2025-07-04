#include <Arduino.h>
#include "data_processing.h"

#include <DallasTemperature.h>
#include "config.h"
#include "EmonLib.h"

// Declare global objects and variables defined in the main .ino file as 'extern'.
// We still need extern for the sensor objects themselves, as they represent hardware.

// Objects
extern DallasTemperature tempSensors;
extern EnergyMonitor fanMonitor;
extern EnergyMonitor compressorMonitor;
extern EnergyMonitor pumpsMonitor;

// Sensor Addresses
extern DeviceAddress returnAirSensorAddress;
extern DeviceAddress supplyAirSensorAddress;

// --- Function Implementations ---

void readTemperatureSensors(HVACData& data) {
  tempSensors.requestTemperatures();
  data.returnTempC = tempSensors.getTempC(returnAirSensorAddress);
  data.supplyTempC = tempSensors.getTempC(supplyAirSensorAddress);
  if (data.returnTempC == DEVICE_DISCONNECTED_C) data.returnTempC = -127.0;
  if (data.supplyTempC == DEVICE_DISCONNECTED_C) data.supplyTempC = -127.0;
}

void readCurrentSensors(HVACData& data) {
  // EmonLib calculates RMS current over a number of samples.
  data.fanAmps = fanMonitor.calcIrms(1480);
  data.compressorAmps = compressorMonitor.calcIrms(1480);
  data.geoPumpsAmps = pumpsMonitor.calcIrms(1480);
}

void processSensorData(HVACData& data) {
  // Calculate Delta T
  if (data.returnTempC != -127.0 && data.supplyTempC != -127.0) {
    data.deltaT = data.returnTempC - data.supplyTempC; // Assumes cooling
  } else {
    data.deltaT = 0;
  }

  // Determine Component Status
  data.fanStatus = (data.fanAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";
  data.compressorStatus = (data.compressorAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";
  data.geoPumpsStatus = (data.geoPumpsAmps > AMPS_ON_THRESHOLD) ? "ON" : "OFF";
}

void printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, data.alertStatus.c_str());
}