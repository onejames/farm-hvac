#ifndef CONFIG_H
#define CONFIG_H

/*
  Centralized configuration for the HVAC Monitoring System. These are DECLARED
  here using 'extern' and DEFINED with their values in config.cpp. Any module
  can include this file to get access to the configuration values.
*/

#include "hvac_hardware_types.h" // For DeviceAddress

// Buffer sizes. These must be compile-time constants to be used with std::array.
// The values are based on the project's README.
constexpr int DATA_BUFFER_SIZE = 60;
constexpr int AGGREGATED_DATA_BUFFER_SIZE = 32;

extern const int ONE_WIRE_BUS_PIN;
extern const int FAN_CT_PIN;
extern const int COMPRESSOR_CT_PIN;
extern const int PUMPS_CT_PIN;
extern const float AMPS_ON_THRESHOLD;
extern const float CT_CALIBRATION;
extern const unsigned int ADC_SAMPLES;
extern const unsigned long SENSOR_READ_INTERVAL_MS;
extern const float LOW_DELTA_T_THRESHOLD;
extern const unsigned int LOW_DELTA_T_DURATION_S;
extern const unsigned int NO_AIRFLOW_DURATION_S;
extern const unsigned int TEMP_SENSOR_DISCONNECTED_DURATION_S;

extern const unsigned int WATCHDOG_TIMEOUT_S;

extern const int I2C_SDA_PIN;
extern const int I2C_SCL_PIN;

extern const DeviceAddress returnAirSensorAddress;
extern const DeviceAddress supplyAirSensorAddress;

#endif // CONFIG_H