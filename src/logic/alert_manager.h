#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H

#include "hvac_data.h"
#include "config.h"
#include <array>

class AlertManager {
public:
    static AlertStatus checkAlerts(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer);
};

#endif // ALERT_MANAGER_H