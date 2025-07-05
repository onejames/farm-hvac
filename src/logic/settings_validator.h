#ifndef SETTINGS_VALIDATOR_H
#define SETTINGS_VALIDATOR_H

#include "config/config_manager.h" // For AppConfig
#include <ArduinoJson.h>

#ifndef ARDUINO
#include <string>
using String = std::string;
#endif

struct ValidationResult {
    bool success;
    String message;
};

class SettingsValidator {
public:
    static ValidationResult validateAndApply(const JsonObject& jsonObj, AppConfig& config);
};

#endif // SETTINGS_VALIDATOR_H