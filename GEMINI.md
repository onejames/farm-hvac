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

The project follows a modular, object-oriented design to separate concerns. All source code is in the `src/` directory.

*   **`main.cpp`**:
    *   The main application entry point (`setup()` and `loop()`). It creates and runs the main `Application` object.

*   **`hvac_data.h`**:
    *   Defines the `HVACData` struct, a central data container passed between modules.

*   **`data_processing.cpp` / `.h`**:
    *   The `DataManager` class encapsulates all logic for reading sensors and processing raw data into meaningful state.

*   **`network_manager.cpp` / `.h`**:
    *   The `NetworkManager` class manages all network tasks: Wi-Fi, the local web server, and AWS IoT MQTT communication.

*   **`config.h`**:
    *   A centralized header file that **declares** all global configuration constants using `extern`. This allows any module to `#include` it and be aware of the configuration variables.

*   **`config.cpp`**:
    *   **Defines** the actual values for the constants declared in `config.h`.

*   **`secrets.h`**:
    *   Stores all sensitive information: Wi-Fi credentials and AWS IoT certificates. This file is listed in `.gitignore` and should not be committed to version control.

## 4. Configuration Management

The project uses a "declare in header, define in source" pattern for global configuration.
1.  **Declaration (`src/config.h`)**: Constants are declared as `extern const ...`.
2.  **Definition (`src/config.cpp`)**: The same constants are defined with their actual values.

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