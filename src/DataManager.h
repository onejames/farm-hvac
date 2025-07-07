#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "hvac_data.h"
#include "hvac_hardware_types.h"

class IHardwareManager; // Forward declaration

class DataManager {
public:
    explicit DataManager(IHardwareManager& hardwareManager,
                         const DeviceAddress& returnAddr,
                         const DeviceAddress& supplyAddr);

    void readAndProcessData(HVACData& data, unsigned int adcSamples, float ampsOnThreshold);

private:
    IHardwareManager& _hardwareManager;
    const DeviceAddress& _returnAirSensorAddress;
    const DeviceAddress& _supplyAirSensorAddress;
};

#endif // DATA_PROCESSING_H