#include "config.h"

// This file provides the definitions for the configuration constants
// that are declared as 'extern' in config.h.

// Hardware Pins
const int ONE_WIRE_BUS_PIN = 4;
const int FAN_CT_PIN = 34;
const int COMPRESSOR_CT_PIN = 35;
const int PUMPS_CT_PIN = 32;

// Application Logic
const float AMPS_ON_THRESHOLD = 0.5f;
const float CT_CALIBRATION = 60.606;
const unsigned int ADC_SAMPLES = 1480;
const unsigned long SENSOR_READ_INTERVAL_MS = 5000;

// Alerting Thresholds
const float LOW_DELTA_T_THRESHOLD = 2.0f;      // Degrees C
const unsigned int LOW_DELTA_T_DURATION_S = 300; // 5 minutes
const unsigned int NO_AIRFLOW_DURATION_S = 60;   // 1 minute

// Watchdog Timer
const unsigned int WATCHDOG_TIMEOUT_S = 15; // seconds

// I2C Pins for OLED Display
const int I2C_SDA_PIN = 21;
const int I2C_SCL_PIN = 22;

const DeviceAddress returnAirSensorAddress = {0x28, 0xFF, 0x64, 0x1E, 0x54, 0x3F, 0x2A, 0x9A};
const DeviceAddress supplyAirSensorAddress = {0x28, 0xFF, 0x64, 0x1E, 0x55, 0x0A, 0x3C, 0x5A};