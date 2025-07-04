#ifndef HVAC_DATA_H
#define HVAC_DATA_H

#include <Arduino.h> // For String type

// A struct to hold all the state and sensor data for the HVAC system.
// This avoids using many global variables and allows us to pass all data
// as a single unit.
struct HVACData {
  // Sensor Readings
  float returnTempC = -127.0;
  float supplyTempC = -127.0;
  float fanAmps = 0.0;
  float compressorAmps = 0.0;
  float geoPumpsAmps = 0.0;
  String airflowStatus = "UNKNOWN";

  // Calculated/Derived State
  float deltaT = 0.0;
  String fanStatus = "OFF";
  String compressorStatus = "OFF";
  String geoPumpsStatus = "OFF";
  String alertStatus = "NONE";
};

#endif // HVAC_DATA_H