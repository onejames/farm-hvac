/*
  HVAC Monitoring System - v1.1 Refactored

  This version has been refactored for improved modularity and code quality.
  It uses a central data structure (HVACData) to pass state between modules,
  eliminating the need for numerous 'extern' global variables.

  Modules:
  - data_processing: Handles all sensor reading and data interpretation.
  - network_manager: Handles Wi-Fi, Web Server, and AWS MQTT communication.
  - secrets.h: Stores all sensitive credentials.
*/

// -------------------
// LIBRARIES & MODULES
// -------------------
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "EmonLib.h"
#include <SPIFFS.h>
#include <PubSubClient.h>

#include "config.h"
#include "hvac_data.h"
#include "data_processing.h"
#include "network_manager.h"

// -------------------
// CONFIGURATION DEFINITIONS
// -------------------
// These constants are DECLARED as 'extern' in config.h and DEFINED here.
// This ensures there is only one copy of each constant in the final program.
const int ONE_WIRE_BUS_PIN = 4;
const int FAN_CT_PIN = 34;
const int COMPRESSOR_CT_PIN = 35;
const int PUMPS_CT_PIN = 32;
const float AMPS_ON_THRESHOLD = 0.5;
const float CT_CALIBRATION = 60.6;
const unsigned long SENSOR_READ_INTERVAL_MS = 5000;
const int ADC_SAMPLES_COUNT = 1480;

// -------------------
// GLOBAL OBJECTS (Hardware & Network Clients)
// -------------------
WiFiClientSecure net;
PubSubClient client(net);
AsyncWebServer server(80);
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature tempSensors(&oneWire);
EnergyMonitor fanMonitor;
EnergyMonitor compressorMonitor;
EnergyMonitor pumpsMonitor;

// Sensor Addresses
DeviceAddress returnAirSensorAddress = { 0x28, 0xFF, 0x64, 0x1E, 0x34, 0x1A, 0x2, 0x9A };
DeviceAddress supplyAirSensorAddress = { 0x28, 0xFF, 0x64, 0x1E, 0x34, 0x1A, 0x2, 0x9B };

// -------------------
// GLOBAL STATE
// -------------------
// A single struct to hold all application state. This is passed to modules.
HVACData hvacData;
unsigned long lastSensorReadTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n[SETUP] HVAC Monitor Starting Up...");

  // -- Initialize Hardware-related objects
  tempSensors.begin();
  fanMonitor.current(FAN_CT_PIN, CT_CALIBRATION);
  compressorMonitor.current(COMPRESSOR_CT_PIN, CT_CALIBRATION);
  pumpsMonitor.current(PUMPS_CT_PIN, CT_CALIBRATION);
  Serial.println("[SETUP] All sensors initialized.");

  // -- Initialize Network and Web Services
  setupNetwork();
  setupWebServer(hvacData); // Pass our data struct to the web server setup
}

void loop() {
  // Non-blocking timer for sensor reads
  if (millis() - lastSensorReadTime >= SENSOR_READ_INTERVAL_MS) {
    lastSensorReadTime = millis();

    readTemperatureSensors(hvacData, tempSensors, returnAirSensorAddress, supplyAirSensorAddress);
    readCurrentSensors(hvacData, fanMonitor, compressorMonitor, pumpsMonitor);
    processSensorData(hvacData); // No objects needed here, just the data
    publishMessage(hvacData);
    printStatus(hvacData);
  }

  // Keep network clients running
  handleMqttClient();
}