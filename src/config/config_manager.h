#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

// A struct to hold all runtime-configurable settings.
struct AppConfig {
    float lowDeltaTThreshold;
    unsigned int lowDeltaTDurationS;
    unsigned int noAirflowDurationS;
    unsigned int tempSensorDisconnectedDurationS;
};

class ConfigManager {
public:
    void load();
    void save();
    void remove();
    AppConfig& getConfig();

private:
    AppConfig _config;
};

#endif // CONFIG_MANAGER_H