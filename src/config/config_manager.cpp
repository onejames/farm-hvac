#include "config_manager.h"
#include "config.h" // For default values
#include <SPIFFS.h>
#include <ArduinoJson.h>

const char* CONFIG_FILE = "/config.json";

void ConfigManager::load() {
    // Set defaults first in case loading fails
    _config.lowDeltaTThreshold = LOW_DELTA_T_THRESHOLD;
    _config.lowDeltaTDurationS = LOW_DELTA_T_DURATION_S;
    _config.noAirflowDurationS = NO_AIRFLOW_DURATION_S;

    if (SPIFFS.begin(true) && SPIFFS.exists(CONFIG_FILE)) {
        File configFile = SPIFFS.open(CONFIG_FILE, "r");
        if (configFile) {
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, configFile);
            if (!error) {
                _config.lowDeltaTThreshold = doc["lowDeltaTThreshold"] | LOW_DELTA_T_THRESHOLD;
                _config.lowDeltaTDurationS = doc["lowDeltaTDurationS"] | LOW_DELTA_T_DURATION_S;
                _config.noAirflowDurationS = doc["noAirflowDurationS"] | NO_AIRFLOW_DURATION_S;
                Serial.println("Loaded configuration from SPIFFS.");
            } else {
                Serial.println("Failed to parse config file, using defaults.");
            }
            configFile.close();
        }
    } else {
        Serial.println("Config file not found, creating with default values.");
        save();
    }
}

void ConfigManager::save() {
    File configFile = SPIFFS.open(CONFIG_FILE, "w");
    if (!configFile) {
        Serial.println("Failed to create config file for saving.");
        return;
    }

    JsonDocument doc;
    doc["lowDeltaTThreshold"] = _config.lowDeltaTThreshold;
    doc["lowDeltaTDurationS"] = _config.lowDeltaTDurationS;
    doc["noAirflowDurationS"] = _config.noAirflowDurationS;

    if (serializeJson(doc, configFile) == 0) {
        Serial.println("Failed to write to config file.");
    } else {
        Serial.println("Configuration saved to SPIFFS.");
    }
    configFile.close();
}

AppConfig& ConfigManager::getConfig() {
    return _config;
}