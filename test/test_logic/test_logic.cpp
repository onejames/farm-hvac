#include <unity.h>
#include "hvac_data.h"
#include "logic/json_builder.h"
#include <ArduinoJson.h>

// The setUp and tearDown functions are called before and after each test.
void setUp(void) {
    // set up resources
}

void tearDown(void) {
    // clean up resources
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

    const char* version = "v-test";
    const char* buildDate = "2024-01-01";
    char buffer[512];

    // 2. Act
    size_t length = JsonBuilder::buildPayload(data, version, buildDate, buffer, sizeof(buffer));

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
    TEST_ASSERT_EQUAL_STRING("v-test", doc["version"]);
    TEST_ASSERT_EQUAL_STRING("2024-01-01", doc["buildDate"]);
}

// This main function is the entry point for this specific test suite.
int main(int argc, char **argv) {
    UNITY_BEGIN();
    // Run JsonBuilder tests
    RUN_TEST(test_buildPayload_creates_correct_json);

    return UNITY_END();
}