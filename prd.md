# Product Requirements: ESP32-Based HVAC Monitor

**Version:** 1.0

## 1. Introduction & Vision

This document outlines the requirements for an IoT-based HVAC monitoring system. The system will use an ESP32 microcontroller to collect real-time data from a residential or light commercial HVAC unit, including temperatures, airflow, and the operational status of key components. The primary goal is to provide homeowners or technicians with actionable insights into the system's performance, efficiency, and health, enabling proactive maintenance and early fault detection. PlatformIO will be used to manage libraries, requirements, and build processes in VS Code.

## 2. Project Goals & Objectives

*   **Goal 1: Provide Real-Time System Visibility.** Give users a live dashboard of the HVAC system's core metrics.
    *   *Objective:* Display current supply and return air temperatures.
    *   *Objective:* Show real-time airflow status.
    *   *Objective:* Clearly indicate whether the fan, compressor, and geothermal pumps are ON or OFF.
*   **Goal 2: Enhance System Diagnostics.** Help identify common HVAC problems automatically.
    *   *Objective:* Calculate and display the temperature differential (Delta T) between supply and return air.
    *   *Objective:* Detect and flag anomalies, such as the fan running with no detectable airflow.
*   **Goal 3: Enable Remote Monitoring.** Make the data accessible over the local network.
    *   *Objective:* Host a simple web page on the ESP32 that displays all sensor data.
    *   *Objective:* Ensure the device can connect reliably to a standard Wi-Fi network.

## 3. User Personas

*   **The Tech-Savvy Homeowner:** Wants to monitor their HVAC system for efficiency, catch problems early to avoid costly repairs, and understand their home's energy usage better. They are comfortable with web interfaces and basic network concepts.
*   **The HVAC Technician:** Uses the device as a diagnostic tool during service calls to get quick, accurate readings without needing multiple separate instruments.

## 4. Functional Requirements

#### FR-1: Sensor Integration & Data Acquisition
The system must interface with and read data from the following sensors:
*   **FR-1.1: Temperature:** Two digital temperature sensors (e.g., DS18B20) to measure:
    *   Return air temperature (air going into the HVAC unit).
    *   Supply air temperature (air coming out of the HVAC unit).
*   **FR-1.2: Airflow:** One airflow monitor (e.g., FS3000-1005) to confirm that air is moving through the ducts when the fan is active.
*   **FR-1.3: Component Status:** Three non-invasive AC current sensors (e.g., SCT-013 0-100A) to monitor the amperage draw of:
    *   The main circulation fan.
    *   The AC compressor or heat pump.
    *   The geothermal water pumps.

#### FR-2: Core Logic & Processing
*   **FR-2.1: Component State Detection:** The software must interpret the amperage reading from each current sensor. A reading above a configurable threshold (e.g., > 0.5A) indicates the component is "ON". A reading below the threshold indicates "OFF".
*   **FR-2.2: Delta T Calculation:** The system will continuously calculate the difference between the supply and return air temperatures (`Delta T = Return Temp - Supply Temp` for cooling, and vice-versa for heating).
*   **FR-2.3: Data Sampling:** All sensors shall be read at a regular interval, configurable to be no less than every 5 seconds.
*   **FR-2.4: Data Logging:** Store historical data to an SD card or SPIFFS on the ESP32.
*   **FR-2.5: Cloud Integration:** Push data to a cloud IoT service (e.g., MQTT to AWS IoT Core) for long-term storage, graphing, and analysis.

#### FR-3: Connectivity & User Interface
*   **FR-3.1: Wi-Fi Connectivity:** The ESP32 must connect to a user-provided 2.4GHz Wi-Fi network using WPA2 security.
*   **FR-3.2: Web Server:** The ESP32 will host a web page accessible from any device on the same local network.
*   **FR-3.3: Data Display:** The web page must display the following in a clean, readable format:
    *   Return Temperature (°C / °F)
    *   Supply Temperature (°C / °F)
    *   Calculated Delta T
    *   Airflow Status (e.g., "OK" or "No Flow")
    *   Fan Status (ON / OFF)
    *   Compressor Status (ON / OFF)
    *   Geothermal Pump Status (ON / OFF)
    *   System Uptime

#### FR-4: Basic Alerting
*   **FR-4.1: Anomaly Detection:** The system will display a visible "ALERT" status on the web page if critical conflicting conditions are met, such as:
    *   The fan status is "ON" but the airflow status is "No Flow" for more than 60 seconds.
    *   The compressor status is "ON" but the Delta T is below a critical threshold (e.g., < 5°F) for more than 5 minutes.

## 5. Non-Functional Requirements

*   **NFR-1: Reliability:** The device must be able to run continuously without crashing. It should include a watchdog timer to automatically reboot in case of a software freeze.
*   **NFR-2: Usability:** The web interface should require no instructions for a user to understand the current state of their HVAC system.
*   **NFR-3: Configurability:** Key parameters (Wi-Fi credentials, amperage thresholds) should be configurable without needing to re-flash the firmware (e.g., via a configuration file on the device or a one-time setup portal).

## 6. Assumptions

*   A 2.4GHz Wi-Fi network is available at the installation site.
*   The user has the ability to safely install the non-invasive current sensors around the appropriate high-voltage wires.
*   Standard sensor models (DS18B20, SCT-013) will be used. The hardware design will accommodate these choices.

## 7. Future Scope (V2.0)

*   **Push Notifications:** Send alerts to a user's phone via a service like Pushover or IFTTT when an anomaly is detected.
*   **Advanced Analytics:** Use historical data to calculate duty cycles, estimate energy consumption, and predict potential failures.