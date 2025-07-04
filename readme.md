# HVAC Monitoring System for ESP32

**Version:** 0.3

## Introduction

>  
> The pirmary motivation for this was to spend some time with CA to both gain some experience as well as attepmt to assertain if and how it was applicable for existant large code-base projects.
>
>  This was written primairly by Gemini Code Assist in VS Code. That would inclide the bulk of this and other plain-text files.
>  

## Description

A robust monitoring system for a geothermal HVAC unit, built for the ESP32. It reads temperature and current sensors, determines the operational state of components, and publishes the data to AWS IoT Core for analysis and alerting. It also hosts a local web page for real-time status checks.

## Features

*   **Temperature Sensing**: Monitors return and supply air temperatures using DS18B20 sensors.
*   **Current Monitoring**: Uses Current Transformers (CTs) to measure the amperage of the fan, compressor, and geothermal water pumps.
*   **State Analysis**: Determines if components are ON/OFF and calculates the temperature differential (Delta T).
*   **Local Web Interface**: Provides a simple, auto-refreshing web page to view live data from any device on the local network.
*   **Cloud Integration**: Securely publishes all sensor data and system status to AWS IoT Core via MQTT for long-term storage, dashboarding, and alerting.
*   **Modular Codebase**: The code is separated into logical modules for easy maintenance and extension.

## Project Structure

The project is organized into several key modules within the `src/` directory:

*   `main.cpp`: The main entry point. Initializes all hardware and network services and contains the main application loop. It also contains the definitions for global configuration constants.
*   `hvac_data.h`: Defines the `HVACData` struct, a central data container that holds all sensor readings and system state. This avoids the use of many global variables.
*   `data_processing.cpp/.h`: Contains all functions related to reading sensors (temperature and current) and processing that raw data into meaningful state (e.g., calculating Delta T, determining component status).
*   `network_manager.cpp/.h`: Manages all network-related tasks, including Wi-Fi connection, the local web server, and communication with AWS IoT Core.
*   `config.h`: A centralized header file that declares all hardware and application configuration constants.
*   `secrets.h`: A dedicated file for storing sensitive information like Wi-Fi credentials and AWS IoT certificates. **This file is not meant to be committed to version control.**

## Setup and Configuration

To get the project running on your own ESP32, you'll need to configure two main files.

### 1. Credentials (`src/secrets.h`)

This file holds all your sensitive connection details. It is separated from the main logic to prevent accidentally sharing credentials in version control.

Open `src/secrets.h` and fill in the following:

*   `WIFI_SSID` and `WIFI_PASSWORD`: Your Wi-Fi network name and password.
*   `AWS_IOT_ENDPOINT`: Your unique endpoint for AWS IoT Core.
*   `AWS_CERT_CA`: The Amazon Root CA 1 certificate.
*   `AWS_CERT_CRT`: The device certificate for your "Thing".
*   `AWS_CERT_PRIVATE`: The device private key for your "Thing".

### 2. Hardware & Application Config (`src/main.cpp`)

All non-sensitive configuration, like GPIO pin assignments and operational thresholds, are managed in one place. The constants are declared in `src/config.h` so they can be used across the project, but their values are set at the top of `src/main.cpp`.

Key values to review and adjust:

*   `ONE_WIRE_BUS_PIN`: The GPIO pin connected to the data line for the DS18B20 temperature sensors.
*   `FAN_CT_PIN`, `COMPRESSOR_CT_PIN`, `PUMPS_CT_PIN`: The analog GPIO pins connected to the current transformer sensors.
*   `AMPS_ON_THRESHOLD`: The current (in Amps) above which a component is considered "ON".
*   `CT_CALIBRATION`: The calibration value for the EmonLib library, specific to your CT sensors and burden resistor.
*   `SENSOR_READ_INTERVAL_MS`: How often (in milliseconds) to read the sensors and publish data.
*   `returnAirSensorAddress`, `supplyAirSensorAddress`: The unique 1-Wire addresses of your DS18B20 sensors. You will need to run a 1-Wire scanner sketch to find the addresses for your specific sensors.

## Dependencies

This project is built using PlatformIO and relies on the following libraries, which are managed automatically via `platformio.ini`:

*   `OneWire` by Paul Stoffregen
*   `DallasTemperature` by Miles Burton
*   `EmonLib` by openenergymonitor
*   `PubSubClient` by Nick O'Leary
*   `ESPAsyncWebServer` by ESP32Async

## How to Build

1.  Install Visual Studio Code and the PlatformIO IDE extension.
2.  Clone this repository.
3.  Open the repository folder in VS Code.
4.  Complete the steps in the "Setup and Configuration" section above.
5.  Connect your ESP32 board, select the correct COM port, and click the "Upload" button in the PlatformIO toolbar.