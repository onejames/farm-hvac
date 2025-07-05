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
const DeviceAddress returnAirSensorAddress = {0x28, 0xFF, 0x64, 0x1E, 0x54, 0x3F, 0x2A, 0x9A};
const DeviceAddress supplyAirSensorAddress = {0x28, 0xFF, 0x64, 0x1E, 0x55, 0x0A, 0x3C, 0x5A};