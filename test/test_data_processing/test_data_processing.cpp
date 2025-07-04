#include <unity.h>
#include "hvac_data.h"
#include "logic/data_analyzer.h"

// The setUp and tearDown functions are called before and after each test.
void setUp(void) {
    // set up resources
}

void tearDown(void) {
    // clean up resources
}

void test_process_data_calculates_delta_t_correctly(void) {
    HVACData data;
    data.returnTempC = 25.0;
    data.supplyTempC = 18.5;

    DataAnalyzer::process(data, 0.5f);

    TEST_ASSERT_EQUAL_FLOAT(6.5, data.deltaT);
}

void test_process_data_handles_sensor_error_for_delta_t(void) {
    HVACData data;
    data.returnTempC = -127.0; // Error code
    data.supplyTempC = 18.5;

    DataAnalyzer::process(data, 0.5f);

    TEST_ASSERT_EQUAL_FLOAT(0.0, data.deltaT);
}

void test_process_data_sets_status_off_when_amps_are_low(void) {
    HVACData data = {}; // Initialize all fields to 0/empty
    data.fanAmps = 0.1;
    data.compressorAmps = 0.0;
    data.geoPumpsAmps = 0.49;

    DataAnalyzer::process(data, 0.5f);

    TEST_ASSERT_EQUAL_STRING("OFF", data.fanStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("OFF", data.compressorStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("OFF", data.geoPumpsStatus.c_str());
}

void test_process_data_sets_status_on_when_amps_are_high(void) {
    HVACData data = {};
    data.fanAmps = 1.2;
    data.compressorAmps = 5.5;
    data.geoPumpsAmps = 0.51;

    DataAnalyzer::process(data, 0.5f);

    TEST_ASSERT_EQUAL_STRING("ON", data.fanStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("ON", data.compressorStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("ON", data.geoPumpsStatus.c_str());
}

void test_process_data_sets_airflow_and_alert_status_correctly(void) {
    HVACData data = {};
    data.fanAmps = 2.0; // Fan is ON

    DataAnalyzer::process(data, 0.5f);

    // With the fan ON, airflow should be "OK" and there should be no alert.
    TEST_ASSERT_EQUAL_STRING("ON", data.fanStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("OK", data.airflowStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("NONE", data.alertStatus.c_str());

    // Now test with fan OFF
    data.fanAmps = 0.1; // Fan is OFF
    DataAnalyzer::process(data, 0.5f);
    TEST_ASSERT_EQUAL_STRING("OFF", data.fanStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("N/A", data.airflowStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("NONE", data.alertStatus.c_str());
}

// This main function is the entry point for this specific test suite.
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_process_data_calculates_delta_t_correctly);
    RUN_TEST(test_process_data_handles_sensor_error_for_delta_t);
    RUN_TEST(test_process_data_sets_status_off_when_amps_are_low);
    RUN_TEST(test_process_data_sets_status_on_when_amps_are_high);
    RUN_TEST(test_process_data_sets_airflow_and_alert_status_correctly);
    return UNITY_END();
}