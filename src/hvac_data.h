#ifndef HVAC_DATA_H
#define HVAC_DATA_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdint> // For uint32_t in native tests
#endif

#include "hvac_status_types.h"

// A struct to hold all the data for the HVAC system.
// This is used to pass data between modules without using global variables.
struct HVACData {
    bool isInitialized = false;
    float returnTempC = -127.0;
    float supplyTempC = -127.0;
    float deltaT = 0.0;
    double fanAmps = 0.0;
    double compressorAmps = 0.0;
    double geoPumpsAmps = 0.0;
    ComponentStatus fanStatus = ComponentStatus::OFF;
    ComponentStatus compressorStatus = ComponentStatus::OFF;
    ComponentStatus geoPumpsStatus = ComponentStatus::OFF;
    AirflowStatus airflowStatus = AirflowStatus::NA;
    AlertStatus alertStatus = AlertStatus::NONE;
};

// A struct to hold aggregated data over a period of time.
struct AggregatedHVACData {
    uint32_t timestamp = 0; // millis() at time of aggregation
    float avgReturnTempC = 0.0;
    float avgSupplyTempC = 0.0;
    float avgDeltaT = 0.0;
    double avgFanAmps = 0.0;
    double avgCompressorAmps = 0.0;
    double avgGeoPumpsAmps = 0.0;
    ComponentStatus lastFanStatus = ComponentStatus::UNKNOWN;
    ComponentStatus lastCompressorStatus = ComponentStatus::UNKNOWN;
    ComponentStatus lastGeoPumpsStatus = ComponentStatus::UNKNOWN;
};

#endif // HVAC_DATA_H