#include <unity.h>
#include "logic/alert_manager.h"
#include "hvac_data.h"
#include "config.h"
#include <array>

void setUp(void) {}
void tearDown(void) {}

void test_checkAlerts_no_alert_on_normal_conditions() {
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());
    for (int i = 0; i < DATA_BUFFER_SIZE; ++i) {
        buffer[i].isInitialized = true;
        buffer[i].fanStatus = ComponentStatus::ON;
        buffer[i].airflowStatus = AirflowStatus::OK;
        buffer[i].compressorStatus = ComponentStatus::ON;
        buffer[i].deltaT = 5.0f;
    }

    AlertStatus result = AlertManager::checkAlerts(buffer);

    TEST_ASSERT_EQUAL(AlertStatus::NONE, result);
}

void test_checkAlerts_triggers_fan_no_airflow_alert() {
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());
    // Simulate fan on with no airflow for the entire buffer duration
    // (assuming buffer duration > NO_AIRFLOW_DURATION_S)
    for (int i = 0; i < DATA_BUFFER_SIZE; ++i) {
        buffer[i].isInitialized = true;
        buffer[i].fanStatus = ComponentStatus::ON;
        buffer[i].airflowStatus = AirflowStatus::NA; // No airflow
    }

    AlertStatus result = AlertManager::checkAlerts(buffer);

    TEST_ASSERT_EQUAL(AlertStatus::FAN_NO_AIRFLOW, result);
}

void test_checkAlerts_triggers_low_delta_t_alert() {
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());
    // Simulate compressor on with low delta T for the entire buffer duration
    // (assuming buffer duration > LOW_DELTA_T_DURATION_S)
    for (int i = 0; i < DATA_BUFFER_SIZE; ++i) {
        buffer[i].isInitialized = true;
        buffer[i].compressorStatus = ComponentStatus::ON;
        buffer[i].deltaT = LOW_DELTA_T_THRESHOLD - 0.5f; // Below threshold
    }

    AlertStatus result = AlertManager::checkAlerts(buffer);

    TEST_ASSERT_EQUAL(AlertStatus::LOW_DELTA_T, result);
}

void test_checkAlerts_does_not_trigger_if_duration_is_too_short() {
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());
    // Simulate a problem for only one sample
    buffer[0].isInitialized = true;
    buffer[0].fanStatus = ComponentStatus::ON;
    buffer[0].airflowStatus = AirflowStatus::NA;

    AlertStatus result = AlertManager::checkAlerts(buffer);

    TEST_ASSERT_EQUAL(AlertStatus::NONE, result);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_checkAlerts_no_alert_on_normal_conditions);
    RUN_TEST(test_checkAlerts_triggers_fan_no_airflow_alert);
    RUN_TEST(test_checkAlerts_triggers_low_delta_t_alert);
    RUN_TEST(test_checkAlerts_does_not_trigger_if_duration_is_too_short);
    return UNITY_END();
}