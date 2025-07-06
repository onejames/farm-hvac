#include "config_manager.h"
#include "config.h" // For default values
#ifdef ARDUINO
#include <SPIFFS.h>
#include <Arduino.h> // For Serial object
#else
#include <SPIFFS.h> // Use mock for native tests (path is handled by build flags)
#endif
#include <ArduinoJson.h>

const char* CONFIG_FILE = "/config.json";

void ConfigManager::load() {
    // Set defaults first in case loading fails
    _config.lowDeltaTThreshold = LOW_DELTA_T_THRESHOLD;
    _config.lowDeltaTDurationS = LOW_DELTA_T_DURATION_S;
    _config.noAirflowDurationS = NO_AIRFLOW_DURATION_S;
    _config.tempSensorDisconnectedDurationS = TEMP_SENSOR_DISCONNECTED_DURATION_S;

    if (!SPIFFS.exists(CONFIG_FILE)) {
#ifdef ARDUINO
        Serial.println("Config file not found, creating with default values.");
#endif
        save();
        return;
    }

    auto configFile = SPIFFS.open(CONFIG_FILE, "r");
    if (!configFile) {
        // This case is unlikely if exists() passed, but good to handle.
        // We'll just use the defaults already set.
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close(); // Close file as soon as we're done with it.

    if (error) {
#ifdef ARDUINO
        Serial.println("Failed to parse config file, using defaults.");
#endif
        return;
    }

    // If we get here, parsing was successful.
    _config.lowDeltaTThreshold = doc["lowDeltaTThreshold"] | LOW_DELTA_T_THRESHOLD;
    _config.lowDeltaTDurationS = doc["lowDeltaTDurationS"] | LOW_DELTA_T_DURATION_S;
    _config.noAirflowDurationS = doc["noAirflowDurationS"] | NO_AIRFLOW_DURATION_S;
    _config.tempSensorDisconnectedDurationS = doc["tempSensorDisconnectedDurationS"] | TEMP_SENSOR_DISCONNECTED_DURATION_S;
#ifdef ARDUINO
    Serial.println("Loaded configuration from SPIFFS.");
#endif
}

void ConfigManager::save() {
    auto configFile = SPIFFS.open(CONFIG_FILE, "w");
    if (!configFile) {
#ifdef ARDUINO
        Serial.println("Failed to create config file for saving.");
#endif
        return;
    }

    JsonDocument doc;
    doc["lowDeltaTThreshold"] = _config.lowDeltaTThreshold;
    doc["lowDeltaTDurationS"] = _config.lowDeltaTDurationS;
    doc["noAirflowDurationS"] = _config.noAirflowDurationS;
    doc["tempSensorDisconnectedDurationS"] = _config.tempSensorDisconnectedDurationS;

    if (serializeJson(doc, configFile) == 0) {
#ifdef ARDUINO
        Serial.println("Failed to write to config file.");
#endif
    } else {
#ifdef ARDUINO
        Serial.println("Configuration saved to SPIFFS.");
#endif
    }
    configFile.close();
}

void ConfigManager::remove() {
    if (SPIFFS.exists(CONFIG_FILE)) {
        SPIFFS.remove(CONFIG_FILE);
#ifdef ARDUINO
        Serial.println("Configuration file removed.");
#endif
    }
}

AppConfig& ConfigManager::getConfig() {
    return _config;
}