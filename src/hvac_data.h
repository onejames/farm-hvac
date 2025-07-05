#ifndef HVAC_DATA_H
#define HVAC_DATA_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "hvac_status_types.h"

// A struct to hold all the data for the HVAC system.
// This is used to pass data between modules without using global variables.
struct HVACData {
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

#endif // HVAC_DATA_H