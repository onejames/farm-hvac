#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include "hvac_data.h"
#include "interfaces/i_temperature_sensor.h"
#include "interfaces/i_current_sensor.h"

class DataManager {
public:
    DataManager(ITemperatureSensor& tempSensors, ICurrentSensor& fan,
                ICurrentSensor& compressor, ICurrentSensor& pumps,
                const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr);

    void readAllSensors(HVACData& data, unsigned int adcSamples);
    void printStatus(const HVACData& data);

private:
    ITemperatureSensor& _tempSensors;
    ICurrentSensor& _fanMonitor;
    ICurrentSensor& _compressorMonitor;
    ICurrentSensor& _pumpsMonitor;
    const DeviceAddress& _returnAddr;
    const DeviceAddress& _supplyAddr;
};

#endif // DATA_PROCESSING_H