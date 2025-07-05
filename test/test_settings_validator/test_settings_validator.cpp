#include <unity.h>
#include "logic/settings_validator.h"
#include "config/config_manager.h"
#include <ArduinoJson.h>

void setUp(void) {}
void tearDown(void) {}

void test_validateAndApply_accepts_valid_data() {
    AppConfig config;
    JsonDocument doc;
    doc["lowDeltaTThreshold"] = 5.5f;
    doc["lowDeltaTDurationS"] = 120;
    doc["noAirflowDurationS"] = 90;

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_FLOAT(5.5f, config.lowDeltaTThreshold);
    TEST_ASSERT_EQUAL_UINT(120, config.lowDeltaTDurationS);
    TEST_ASSERT_EQUAL_UINT(90, config.noAirflowDurationS);
}

void test_validateAndApply_rejects_low_delta_t_threshold() {
    AppConfig config;
    JsonDocument doc;
    doc["lowDeltaTThreshold"] = -1.0f;

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("Invalid Delta T threshold. Must be between 0.0 and 20.0.", result.message.c_str());
}

void test_validateAndApply_rejects_high_delta_t_threshold() {
    AppConfig config;
    JsonDocument doc;
    doc["lowDeltaTThreshold"] = 21.0f;

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("Invalid Delta T threshold. Must be between 0.0 and 20.0.", result.message.c_str());
}

void test_validateAndApply_rejects_low_duration() {
    AppConfig config;
    JsonDocument doc;
    doc["lowDeltaTDurationS"] = 5;

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("Invalid Delta T duration. Must be between 10 and 3600 seconds.", result.message.c_str());
}

void test_validateAndApply_rejects_high_duration() {
    AppConfig config;
    JsonDocument doc;
    doc["noAirflowDurationS"] = 4000;

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_FALSE(result.success);
    TEST_ASSERT_EQUAL_STRING("Invalid No Airflow duration. Must be between 10 and 3600 seconds.", result.message.c_str());
}

void test_validateAndApply_handles_partial_update() {
    AppConfig config = {2.0f, 300, 60}; // Set initial values

    JsonDocument doc;
    doc["lowDeltaTDurationS"] = 500; // Only update one value

    ValidationResult result = SettingsValidator::validateAndApply(doc.as<JsonObject>(), config);

    TEST_ASSERT_TRUE(result.success);
    TEST_ASSERT_EQUAL_FLOAT(2.0f, config.lowDeltaTThreshold); // Should be unchanged
    TEST_ASSERT_EQUAL_UINT(500, config.lowDeltaTDurationS);   // Should be updated
    TEST_ASSERT_EQUAL_UINT(60, config.noAirflowDurationS);    // Should be unchanged
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_validateAndApply_accepts_valid_data);
    RUN_TEST(test_validateAndApply_rejects_low_delta_t_threshold);
    RUN_TEST(test_validateAndApply_rejects_high_delta_t_threshold);
    RUN_TEST(test_validateAndApply_rejects_low_duration);
    RUN_TEST(test_validateAndApply_rejects_high_duration);
    RUN_TEST(test_validateAndApply_handles_partial_update);
    return UNITY_END();
}