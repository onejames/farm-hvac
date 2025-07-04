#ifndef HVAC_DATA_H
#define HVAC_DATA_H

// Use conditional compilation to include the correct headers for the target environment.
#ifdef ARDUINO
#include <DallasTemperature.h> // Provides String (via Arduino.h) and DeviceAddress
#else
#include <string>    // For the native test environment
#include <cstdint>   // For uint8_t
using String = std::string; // Alias std::string to String for code compatibility
typedef uint8_t DeviceAddress[8]; // Manually define for native tests
#endif

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