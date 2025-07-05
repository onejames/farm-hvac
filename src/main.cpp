#include <Arduino.h>
#include "config.h"
#include "secrets.h"
#include "hvac_data.h"
#include "data_processing.h" // For DataManager
#include "logic/data_analyzer.h" // For DataAnalyzer
#include "network/network_manager.h"
#include "version.h"

// Sensor libraries and adapters
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EmonLib.h>
#include "adapters/dallas_temperature_adapter.h"
#include "adapters/emon_lib_adapter.h"

// Network libraries
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>

// --- Configuration Constants ---
// These values are defined here and declared as 'extern' in config.h
// allowing them to be used across the project.

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

// Global data structure
HVACData hvacData;

// Hardware objects
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature tempSensors(&oneWire);
EnergyMonitor fanMonitor;
EnergyMonitor compressorMonitor;
EnergyMonitor pumpsMonitor;

// Adapter objects to bridge hardware libs to interfaces
DallasTemperatureAdapter tempAdapter(tempSensors);
EmonLibAdapter fanAdapter(fanMonitor);
EmonLibAdapter compressorAdapter(compressorMonitor);
EmonLibAdapter pumpsAdapter(pumpsMonitor);

// DataManager to handle sensor reading logic
DataManager dataManager(tempAdapter, fanAdapter, compressorAdapter, pumpsAdapter, returnAirSensorAddress, supplyAirSensorAddress);

// Network objects
WiFiClientSecure net;
PubSubClient client(net);
AsyncWebServer server(80);

unsigned long lastReadTime = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial); // Wait for serial connection
    Serial.println("Booting HVAC Monitor...");
    Serial.printf("Version: %s, Built: %s\n", FIRMWARE_VERSION, BUILD_DATE);

    // Initialize hardware sensors
    tempSensors.begin();
    fanMonitor.current(FAN_CT_PIN, CT_CALIBRATION);
    compressorMonitor.current(COMPRESSOR_CT_PIN, CT_CALIBRATION);
    pumpsMonitor.current(PUMPS_CT_PIN, CT_CALIBRATION);

    // Initialize network
    setupWiFi();

    // Configure MQTT client
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);
    client.setServer(AWS_IOT_ENDPOINT, 8883);

    // Setup the local web interface
    setupWebInterface();

    Serial.println("Setup complete. Entering main loop.");
}

void loop() {
    handleMqttClient();

    unsigned long currentTime = millis();
    if (currentTime - lastReadTime >= SENSOR_READ_INTERVAL_MS) {
        lastReadTime = currentTime;

        // 1. Read raw sensor data using the DataManager
        dataManager.readAllSensors(hvacData, ADC_SAMPLES);

        // 2. Process raw data to derive state and alerts
        DataAnalyzer::process(hvacData, AMPS_ON_THRESHOLD);

        // 3. Publish data via MQTT
        publishMessage();

        // 4. Print status to serial for debugging
        dataManager.printStatus(hvacData);
    }
}