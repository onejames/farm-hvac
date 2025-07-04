#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <DallasTemperature.h>
#include "EmonLib.h"
#include "hvac_data.h"

// Function declarations for sensor reading and data processing
void readTemperatureSensors(HVACData& data, DallasTemperature& tempSensors,
                            const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr);

void readCurrentSensors(HVACData& data, EnergyMonitor& fan,
                        EnergyMonitor& compressor, EnergyMonitor& pumps);

void processSensorData(HVACData& data); // Combines calculation and status determination
void printStatus(const HVACData& data);

#endif // DATA_PROCESSING_H