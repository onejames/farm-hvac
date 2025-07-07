#include "DataManager.h"
#include "hardware/IHardwareManager.h"
#include "interfaces/i_temperature_sensor.h"
#include "interfaces/i_current_sensor.h"

DataManager::DataManager(IHardwareManager& hardwareManager,
                         const DeviceAddress& returnAddr,
                         const DeviceAddress& supplyAddr)
    : _hardwareManager(hardwareManager),
      _returnAirSensorAddress(returnAddr),
      _supplyAirSensorAddress(supplyAddr)
{}

void DataManager::readAndProcessData(HVACData& data, unsigned int adcSamples, float ampsOnThreshold) {
    ITemperatureSensor& tempSensor = _hardwareManager.getTempAdapter();
    ICurrentSensor& fanSensor = _hardwareManager.getFanAdapter();
    ICurrentSensor& compressorSensor = _hardwareManager.getCompressorAdapter();
    ICurrentSensor& pumpsSensor = _hardwareManager.getPumpsAdapter();

    tempSensor.requestTemperatures();
    data.returnTempC = tempSensor.getTempC(_returnAirSensorAddress);
    data.supplyTempC = tempSensor.getTempC(_supplyAirSensorAddress);

    if (data.returnTempC == -127.0f || data.supplyTempC == -127.0f) {
        data.deltaT = 0.0f;
    } else {
        data.deltaT = data.returnTempC - data.supplyTempC;
    }

    data.fanAmps = fanSensor.calcIrms(adcSamples);
    data.compressorAmps = compressorSensor.calcIrms(adcSamples);
    data.geoPumpsAmps = pumpsSensor.calcIrms(adcSamples);

    data.fanStatus = (data.fanAmps > ampsOnThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;
    data.compressorStatus = (data.compressorAmps > ampsOnThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;
    data.geoPumpsStatus = (data.geoPumpsAmps > ampsOnThreshold) ? ComponentStatus::ON : ComponentStatus::OFF;

    data.isInitialized = true;
}