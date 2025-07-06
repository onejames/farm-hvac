#include <unity.h>
#include "config.h"
#include "logic/alert_manager.h"
#include "hvac_data.h"
#include "config/config_manager.h"
#include <array>

void setUp(void) {}
void tearDown(void) {}

// Helper to create a default config for tests
AppConfig create_test_config() {
    AppConfig config;
    config.lowDeltaTThreshold = 2.0f;
    config.lowDeltaTDurationS = 300;
    config.noAirflowDurationS = 60;
    config.tempSensorDisconnectedDurationS = 30;
    return config;
}

// Helper to fill the buffer with a baseline of normal, initialized data
void fill_buffer_with_normal_data(std::array<HVACData, DATA_BUFFER_SIZE>& buffer) {
    for (auto& data : buffer) {
        data.isInitialized = true;
        data.fanStatus = ComponentStatus::ON;
        data.airflowStatus = AirflowStatus::OK;
        data.compressorStatus = ComponentStatus::ON;
        data.deltaT = 5.0f;
        data.returnTempC = 25.0f;
        data.supplyTempC = 20.0f;
    }
}

void test_checkAlerts_no_alert_on_normal_conditions() {
    AppConfig config = create_test_config();
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    fill_buffer_with_normal_data(buffer);

    AlertStatus result = AlertManager::checkAlerts(buffer, config);

    TEST_ASSERT_EQUAL(AlertStatus::NONE, result);
}

void test_checkAlerts_triggers_fan_no_airflow_alert() {
    AppConfig config = create_test_config();
    config.noAirflowDurationS = (DATA_BUFFER_SIZE * SENSOR_READ_INTERVAL_MS / 1000) - 1; // Ensure duration is met

    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    fill_buffer_with_normal_data(buffer);

    // Introduce the fault condition
    for (auto& data : buffer) {
        data.airflowStatus = AirflowStatus::NA;
    }

    AlertStatus result = AlertManager::checkAlerts(buffer, config);

    TEST_ASSERT_EQUAL(AlertStatus::FAN_NO_AIRFLOW, result);
}

void test_checkAlerts_triggers_low_delta_t_alert() {
    AppConfig config = create_test_config();
    config.lowDeltaTDurationS = (DATA_BUFFER_SIZE * SENSOR_READ_INTERVAL_MS / 1000) - 1; // Ensure duration is met

    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    fill_buffer_with_normal_data(buffer);

    // Introduce the fault condition
    for (auto& data : buffer) {
        data.compressorStatus = ComponentStatus::ON;
        data.deltaT = config.lowDeltaTThreshold - 0.5f; // Below threshold
    }

    AlertStatus result = AlertManager::checkAlerts(buffer, config);

    TEST_ASSERT_EQUAL(AlertStatus::LOW_DELTA_T, result);
}

void test_checkAlerts_does_not_trigger_if_duration_is_too_short() {
    AppConfig config = create_test_config();
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    fill_buffer_with_normal_data(buffer);

    // Simulate a problem for only one sample
    buffer[0].isInitialized = true;
    buffer[0].fanStatus = ComponentStatus::ON;
    buffer[0].airflowStatus = AirflowStatus::NA;

    AlertStatus result = AlertManager::checkAlerts(buffer, config);

    TEST_ASSERT_EQUAL(AlertStatus::NONE, result);
}

void test_checkAlerts_triggers_temp_sensor_disconnected_alert() {
    AppConfig config = create_test_config();
    config.tempSensorDisconnectedDurationS = (DATA_BUFFER_SIZE * SENSOR_READ_INTERVAL_MS / 1000) - 1; // Ensure duration is met

    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    fill_buffer_with_normal_data(buffer);

    // Introduce the fault condition
    for (auto& data : buffer) {
        data.returnTempC = -127.0f; // Disconnected
    }

    AlertStatus result = AlertManager::checkAlerts(buffer, config);

    TEST_ASSERT_EQUAL(AlertStatus::TEMP_SENSOR_DISCONNECTED, result);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_checkAlerts_no_alert_on_normal_conditions);
    RUN_TEST(test_checkAlerts_triggers_fan_no_airflow_alert);
    RUN_TEST(test_checkAlerts_triggers_low_delta_t_alert);
    RUN_TEST(test_checkAlerts_does_not_trigger_if_duration_is_too_short);
    RUN_TEST(test_checkAlerts_triggers_temp_sensor_disconnected_alert);
    return UNITY_END();
}