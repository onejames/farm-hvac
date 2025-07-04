#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "hvac_data.h"

// Function declarations for sensor reading and data processing
void readTemperatureSensors(HVACData& data);
void readCurrentSensors(HVACData& data);
void processSensorData(HVACData& data); // Combines calculation and status determination
void printStatus(const HVACData& data);

#endif // DATA_PROCESSING_H