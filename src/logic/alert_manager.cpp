#include "alert_manager.h"
#include "config/config_manager.h" // For AppConfig struct

AlertStatus AlertManager::checkAlerts(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const AppConfig& config) {
    int fanOnNoAirflowCount = 0;
    int lowDeltaTCount = 0;

    for (const auto& data : dataBuffer) {
        if (!data.isInitialized) {
            continue;
        }

        // Check for Fan ON but no airflow
        if (data.fanStatus == ComponentStatus::ON && data.airflowStatus == AirflowStatus::NA) {
            fanOnNoAirflowCount++;
        }

        // Check for Compressor ON but low Delta T
        if (data.compressorStatus == ComponentStatus::ON && data.deltaT < config.lowDeltaTThreshold) {
            lowDeltaTCount++;
        }
    }

    // Convert counts to duration in seconds
    float fanOnNoAirflowDuration = fanOnNoAirflowCount * (SENSOR_READ_INTERVAL_MS / 1000.0f);
    float lowDeltaTDuration = lowDeltaTCount * (SENSOR_READ_INTERVAL_MS / 1000.0f);

    // Check if durations exceed thresholds
    if (fanOnNoAirflowDuration >= config.noAirflowDurationS) {
        return AlertStatus::FAN_NO_AIRFLOW;
    }

    if (lowDeltaTDuration >= config.lowDeltaTDurationS) {
        return AlertStatus::LOW_DELTA_T;
    }

    return AlertStatus::NONE;
}