#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

// A struct to hold all runtime-configurable settings.
struct AppConfig {
    float lowDeltaTThreshold;
    unsigned int lowDeltaTDurationS;
    unsigned int noAirflowDurationS;
    unsigned int tempSensorDisconnectedDurationS;
};

class IFileSystem; // Forward declaration

class ConfigManager {
public:
    explicit ConfigManager(IFileSystem& fs);
    void load();
    void save();
    void remove();
    [[nodiscard]] AppConfig& getConfig();

private:
    IFileSystem& _fs;
    AppConfig _config;
};

#endif // CONFIG_MANAGER_H