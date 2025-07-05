#ifndef CONFIG_H
#define CONFIG_H

/*
  Centralized configuration for the HVAC Monitoring System.
  These are DECLARED here and DEFINED in main.cpp. Any module can include
  this file to get access to the configuration values.
*/

#include "hvac_hardware_types.h" // For DeviceAddress

extern const int ONE_WIRE_BUS_PIN;
extern const int FAN_CT_PIN;
extern const int COMPRESSOR_CT_PIN;
extern const int PUMPS_CT_PIN;
extern const float AMPS_ON_THRESHOLD;
extern const float CT_CALIBRATION;
extern const unsigned int ADC_SAMPLES;
extern const unsigned long SENSOR_READ_INTERVAL_MS;

extern const int I2C_SDA_PIN;
extern const int I2C_SCL_PIN;

extern const DeviceAddress returnAirSensorAddress;
extern const DeviceAddress supplyAirSensorAddress;

#endif // CONFIG_H