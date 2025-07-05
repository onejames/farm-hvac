#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "hvac_data.h"
#include "config.h"
#include <array>

struct AppConfig; // Forward declaration

class AlertManager {
public:
    static AlertStatus checkAlerts(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const AppConfig& config);
};

#endif // ALERT_MANAGER_H