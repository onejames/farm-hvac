#include "settings_validator.h"

ValidationResult SettingsValidator::validateAndApply(const JsonObject& jsonObj, AppConfig& config) {
    if (!jsonObj["lowDeltaTThreshold"].isNull()) {
        float val = jsonObj["lowDeltaTThreshold"].as<float>();
        if (val < 0.0f || val > 20.0f) {
            return {false, "Invalid Delta T threshold. Must be between 0.0 and 20.0."};
        }
        config.lowDeltaTThreshold = val;
    }

    if (!jsonObj["lowDeltaTDurationS"].isNull()) {
        unsigned int val = jsonObj["lowDeltaTDurationS"].as<unsigned int>();
        if (val < 10 || val > 3600) {
            return {false, "Invalid Delta T duration. Must be between 10 and 3600 seconds."};
        }
        config.lowDeltaTDurationS = val;
    }

    if (!jsonObj["noAirflowDurationS"].isNull()) {
        unsigned int val = jsonObj["noAirflowDurationS"].as<unsigned int>();
        if (val < 10 || val > 3600) {
            return {false, "Invalid No Airflow duration. Must be between 10 and 3600 seconds."};
        }
        config.noAirflowDurationS = val;
    }

    if (!jsonObj["tempSensorDisconnectedDurationS"].isNull()) {
        unsigned int val = jsonObj["tempSensorDisconnectedDurationS"].as<unsigned int>();
        if (val < 10 || val > 3600) {
            return {false, "Invalid Temp Sensor Disconnected duration. Must be between 10 and 3600 seconds."};
        }
        config.tempSensorDisconnectedDurationS = val;
    }

    return {true, "Settings applied."};
}