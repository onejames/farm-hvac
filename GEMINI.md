# Project Context: ESP32 HVAC Monitor

This document provides the necessary context for an AI coding assistant to understand the farm-hvac project.

## 1. Project Vision & Goal

The project is an IoT monitoring system for a geothermal HVAC unit, built on an ESP32.

The primary goals are:
*   **Real-Time Visibility**: Provide a live view of the system's core metrics (PRD Goal 1).
*   **Enhanced Diagnostics**: Automatically calculate key performance indicators like temperature differential (Delta T) to help identify problems (PRD Goal 2).
*   **Remote Monitoring**: Make data accessible via a local web interface and push it to the cloud (AWS IoT Core) for long-term storage and analysis (PRD Goal 3).
*   **Data Logging**: Implement on-device data logging to SPIFFS or an SD card for historical data analysis (as per PRD FR-2.4).

## 2. Core Technologies & Dependencies

*   **Hardware**: ESP32 Dev Kit
*   **Framework**: PlatformIO with Arduino
*   **Sensors**:
    *   **Temperature**: DS18B20 digital sensors on a 1-Wire bus (PRD FR-1.1).
    *   **Current**: SCT-013 style non-invasive Current Transformers (CTs) (PRD FR-1.3).
*   **Key Libraries**:
    *   `DallasTemperature`: For DS18B20 sensors.
    *   `EmonLib`: For processing data from the CT sensors.
    *   `ESPAsyncWebServer`: For the non-blocking local web server.
    *   `PubSubClient`: For MQTT communication with AWS IoT.

## 3. Software Architecture & Code Structure

The project follows a modular design to separate concerns. All source code is in the `src/` directory.

*   **`main.cpp`**:
    *   The main application entry point (`setup()` and `loop()`).
    *   **Defines** all global configuration constants (pins, thresholds).
    *   **Instantiates** all global hardware and network client objects (e.g., `DallasTemperature`, `PubSubClient`, `AsyncWebServer`).

*   **`hvac_data.h`**:
    *   Defines the `HVACData` struct. This is a critical design choice to act as a central data container, passed by reference between modules to avoid excessive global variables.

*   **`data_processing.cpp` / `.h`**:
    *   Contains all logic for reading from physical sensors.
    *   Performs data analysis (e.g., calculating Delta T, determining component ON/OFF status).
    *   Uses `extern` to access the global sensor objects (e.g., `tempSensors`, `fanMonitor`) that are instantiated in `main.cpp`.

*   **`network_manager.cpp` / `.h`**:
    *   Manages all network functionality: Wi-Fi connection, the local web server, and AWS IoT MQTT communication.
    *   Builds the HTML for the local dashboard and the JSON payload for MQTT.
    *   Uses `extern` to access the global network client objects (`client`, `server`) instantiated in `main.cpp`.

*   **`config.h`**:
    *   A centralized header file that **declares** all global configuration constants using `extern`. This allows any module to `#include` it and be aware of the configuration variables.

*   **`secrets.h`**:
    *   Stores all sensitive information: Wi-Fi credentials and AWS IoT certificates. This file is listed in `.gitignore` and should not be committed to version control.

## 4. Configuration Management

The project uses a "declare in header, define in source" pattern for configuration.

1.  **Declaration (`src/config.h`)**: Constants are declared as `extern const ...` without a value.
2.  **Definition (`src/main.cpp`)**: The same constants are defined with their actual values. This ensures there is only one instance of each constant in the final compiled program.

## 5. Core Logic Flow

The main `loop()` in `main.cpp` is non-blocking and timer-based.

1.  Every `SENSOR_READ_INTERVAL_MS` (PRD FR-2.3), the following sequence occurs:
2.  `readTemperatureSensors()` and `readCurrentSensors()` are called to populate the `hvacData` struct with raw data.
3.  `processSensorData()` is called to calculate derived values (like `deltaT` and component status strings) and update `hvacData`.
4.  `publishMessage()` serializes the `hvacData` struct into a JSON string and sends it to AWS IoT (PRD FR-2.5).
5.  `printStatus()` outputs a summary to the Serial monitor for debugging.
6.  The `handleMqttClient()` function is called on every loop iteration to maintain the MQTT connection state.

## 6. Key Functional Requirements Summary

*   Read supply and return air temperatures (PRD FR-1.1).
*   Read amperage for the fan, compressor, and water pumps (PRD FR-1.3).
*   Determine if a component is "ON" or "OFF" based on the `AMPS_ON_THRESHOLD` (PRD FR-2.1).
*   Host a local web page that auto-refreshes with the latest data (PRD FR-3.2, FR-3.3).
*   Publish a complete JSON payload of all data to the `hvac/data` MQTT topic on AWS IoT (PRD FR-2.5).

## 7. Future Scope

*   Implement on-device alerting for anomaly detection (e.g., fan on but no airflow) (PRD FR-4.1).
*   Add push notifications for alerts (PRD V2.0 Scope).
*   Develop a configuration portal to set Wi-Fi and other parameters without re-flashing (PRD NFR-3).