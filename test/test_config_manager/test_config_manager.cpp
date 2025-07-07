#include <unity.h>
#include "config/config_manager.h"
#include "config.h"
#include "mocks/MockFileSystem.h"
#include <ArduinoJson.h>

void setUp(void) {}

void tearDown(void) {}

void test_load_creates_default_file_if_not_exists() {
    MockFileSystem mockFS;
    ConfigManager cm(mockFS);
    cm.load(); // Should detect file doesn't exist and call save()

    // Verify that the config has default values
    TEST_ASSERT_EQUAL_FLOAT(LOW_DELTA_T_THRESHOLD, cm.getConfig().lowDeltaTThreshold);

    // Verify that save() was called and wrote the default config to the mock file
    JsonDocument doc;
    deserializeJson(doc, mockFS.getFileContent("/config.json"));
    TEST_ASSERT_EQUAL_FLOAT(LOW_DELTA_T_THRESHOLD, doc["lowDeltaTThreshold"]);
}

void test_load_parses_existing_file() {
    MockFileSystem mockFS;
    ConfigManager cm(mockFS);

    // Programmatically create the test JSON to make the test more robust and maintainable.
    JsonDocument doc;
    doc["lowDeltaTThreshold"] = 5.5f;
    doc["lowDeltaTDurationS"] = 500;
    doc["noAirflowDurationS"] = 100;
    doc["tempSensorDisconnectedDurationS"] = 40;
    std::string json_string;
    serializeJson(doc, json_string);
    mockFS.setFileContent("/config.json", json_string);

    cm.load();

    TEST_ASSERT_EQUAL_FLOAT(5.5f, cm.getConfig().lowDeltaTThreshold);
    TEST_ASSERT_EQUAL_UINT(500, cm.getConfig().lowDeltaTDurationS);
    TEST_ASSERT_EQUAL_UINT(100, cm.getConfig().noAirflowDurationS);
    TEST_ASSERT_EQUAL_UINT(40, cm.getConfig().tempSensorDisconnectedDurationS);
}

void test_save_writes_correct_json() {
    MockFileSystem mockFS;
    ConfigManager cm(mockFS);
    cm.getConfig().lowDeltaTThreshold = 9.9f;
    cm.getConfig().lowDeltaTDurationS = 999;

    cm.save();

    JsonDocument doc;
    deserializeJson(doc, mockFS.getFileContent("/config.json"));
    TEST_ASSERT_EQUAL_FLOAT(9.9f, doc["lowDeltaTThreshold"]);
    TEST_ASSERT_EQUAL_UINT(999, doc["lowDeltaTDurationS"]);
}

void test_remove_deletes_file() {
    MockFileSystem mockFS;
    ConfigManager cm(mockFS);
    // Simulate that the file exists initially
    mockFS.setFileContent("/config.json", "{\"some\":\"data\"}");

    cm.remove();

    // After remove is called, the mock should report that the file no longer exists.
    TEST_ASSERT_FALSE(mockFS.exists("/config.json"));
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_load_creates_default_file_if_not_exists);
    RUN_TEST(test_load_parses_existing_file);
    RUN_TEST(test_save_writes_correct_json);
    RUN_TEST(test_remove_deletes_file);
    return UNITY_END();
}