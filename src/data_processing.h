#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <DallasTemperature.h>
#include "EmonLib.h"
#include "hvac_data.h"

class DataManager {
public:
    DataManager(DallasTemperature& tempSensors, EnergyMonitor& fan,
                EnergyMonitor& compressor, EnergyMonitor& pumps,
                const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr);

    void readAllSensors(HVACData& data);
    void processSensorData(HVACData& data);
    void printStatus(const HVACData& data);

private:
    DallasTemperature& _tempSensors;
    EnergyMonitor& _fanMonitor;
    EnergyMonitor& _compressorMonitor;
    EnergyMonitor& _pumpsMonitor;
    const DeviceAddress& _returnAddr;
    const DeviceAddress& _supplyAddr;
};

#endif // DATA_PROCESSING_H