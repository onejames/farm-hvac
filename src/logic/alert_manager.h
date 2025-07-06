#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "config.h"
#include "hvac_data.h"
#include <array>

struct AppConfig; // Forward declaration

namespace AlertManager {
    AlertStatus checkAlerts(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const AppConfig& config);
} // namespace AlertManager

#endif // ALERT_MANAGER_H