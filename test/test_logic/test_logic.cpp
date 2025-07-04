#include <unity.h>
#include "hvac_data.h"
#include "logic/data_analyzer.h"
#include "logic/json_builder.h"
#include <ArduinoJson.h>

// The setUp and tearDown functions are called before and after each test.
void setUp(void) {
    // set up resources
}

void tearDown(void) {
    // clean up resources
}

// --- DataAnalyzer Tests ---

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
    TEST_ASSERT_EQUAL_STRING("ON", data.fanStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("OK", data.airflowStatus.c_str());
    TEST_ASSERT_EQUAL_STRING("NONE", data.alertStatus.c_str());
}

// --- JsonBuilder Tests ---

void test_buildPayload_creates_correct_json(void) {
    // 1. Arrange
    HVACData data;
    data.returnTempC = 24.5;
    data.supplyTempC = 19.0;
    data.deltaT = 5.5;
    data.fanAmps = 1.23;
    data.compressorAmps = 4.56;
    data.geoPumpsAmps = 0.78;
    data.fanStatus = "ON";
    data.compressorStatus = "ON";
    data.geoPumpsStatus = "OFF";
    data.airflowStatus = "OK";
    data.alertStatus = "NONE";

    char buffer[256];

    // 2. Act
    size_t length = JsonBuilder::buildPayload(data, buffer, sizeof(buffer));

    // 3. Assert
    TEST_ASSERT_GREATER_THAN(0, length);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buffer, length);
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    // Verify all fields in the JSON payload for completeness
    TEST_ASSERT_EQUAL_FLOAT(24.5, doc["returnTempC"]);
    TEST_ASSERT_EQUAL_FLOAT(19.0, doc["supplyTempC"]);
    TEST_ASSERT_EQUAL_FLOAT(5.5, doc["deltaT"]);
    TEST_ASSERT_EQUAL_FLOAT(1.23, doc["fanAmps"]);
    TEST_ASSERT_EQUAL_FLOAT(4.56, doc["compressorAmps"]);
    TEST_ASSERT_EQUAL_FLOAT(0.78, doc["geoPumpsAmps"]);
    TEST_ASSERT_EQUAL_STRING("ON", doc["fanStatus"]);
    TEST_ASSERT_EQUAL_STRING("ON", doc["compressorStatus"]);
    TEST_ASSERT_EQUAL_STRING("OFF", doc["geoPumpsStatus"]);
    TEST_ASSERT_EQUAL_STRING("OK", doc["airflowStatus"]);
    TEST_ASSERT_EQUAL_STRING("NONE", doc["alertStatus"]);
}

// This main function is the entry point for this specific test suite.
int main(int argc, char **argv) {
    UNITY_BEGIN();

    // Run DataAnalyzer tests
    RUN_TEST(test_process_data_calculates_delta_t_correctly);
    RUN_TEST(test_process_data_handles_sensor_error_for_delta_t);
    RUN_TEST(test_process_data_sets_status_off_when_amps_are_low);
    RUN_TEST(test_process_data_sets_status_on_when_amps_are_high);
    RUN_TEST(test_process_data_sets_airflow_and_alert_status_correctly);

    // Run JsonBuilder tests
    RUN_TEST(test_buildPayload_creates_correct_json);

    return UNITY_END();
}