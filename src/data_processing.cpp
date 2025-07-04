#include "data_processing.h"

DataManager::DataManager(ITemperatureSensor& tempSensors, ICurrentSensor& fan,
                         ICurrentSensor& compressor, ICurrentSensor& pumps,
                         const DeviceAddress& returnAddr, const DeviceAddress& supplyAddr)
    : _tempSensors(tempSensors),
      _fanMonitor(fan),
      _compressorMonitor(compressor),
      _pumpsMonitor(pumps),
      _returnAddr(returnAddr),
      _supplyAddr(supplyAddr) {}

void DataManager::readAllSensors(HVACData& data, unsigned int adcSamples) {
  // Read Temperatures
  _tempSensors.requestTemperatures();
  data.returnTempC = _tempSensors.getTempC(_returnAddr);
  data.supplyTempC = _tempSensors.getTempC(_supplyAddr);

  // Read Currents
  data.fanAmps = _fanMonitor.calcIrms(adcSamples);
  data.compressorAmps = _compressorMonitor.calcIrms(adcSamples);
  data.geoPumpsAmps = _pumpsMonitor.calcIrms(adcSamples);
}

#ifdef ARDUINO
#include <Arduino.h> // For Serial.printf
void DataManager::printStatus(const HVACData& data) {
  Serial.printf("Temps: Ret=%.1fC, Sup=%.1fC, dT=%.1fC | Amps: Fan=%.2f, Comp=%.2f, Pump=%.2f | Alert: %s\n",
    data.returnTempC, data.supplyTempC, data.deltaT, data.fanAmps, data.compressorAmps, data.geoPumpsAmps, data.alertStatus.c_str());
}
#endif // ARDUINO