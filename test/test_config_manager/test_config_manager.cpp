#include <unity.h>
#include "config/config_manager.h"
#include "config.h"
#include <ArduinoJson.h>

// --- Mock SPIFFS Implementation for Native Testing ---
// This code is only compiled when running tests on the host machine (not on the ESP32).
// It simulates the behavior of the SPIFFS filesystem in memory.
#ifndef ARDUINO

#include <string>
#include <iostream>

namespace {
    // Use static variables to hold the state of our mock filesystem
    std::string mock_file_content;
    bool mock_file_exists = false;
    size_t mock_file_read_pos = 0;
}

class MockFile {
public:
    // Simulate reading from the file
    size_t readBytes(char* buffer, size_t length) {
        size_t bytes_to_read = 0;
        if (mock_file_read_pos < mock_file_content.length()) {
            bytes_to_read = std::min(length, mock_file_content.length() - mock_file_read_pos);
            memcpy(buffer, mock_file_content.c_str() + mock_file_read_pos, bytes_to_read);
            mock_file_read_pos += bytes_to_read;
        }
        return bytes_to_read;
    }

    // Simulate writing to the file
    size_t print(const char* s) {
        mock_file_content += s;
        return strlen(s);
    }

    // Required by ArduinoJson, returns true if the object is valid
    operator bool() const { return true; }
    void close() {}
};

class MockSPIFFS {
public:
    bool begin(bool formatOnFail = false) { return true; }
    bool exists(const char* path) { return mock_file_exists; }
    MockFile open(const char* path, const char* mode) {
        if (strcmp(mode, "w") == 0) {
            mock_file_content.clear();
        }
        mock_file_read_pos = 0;
        return MockFile();
    }
};

// Replace the real SPIFFS with our mock version for native tests
MockSPIFFS SPIFFS;

#endif // !ARDUINO
// --- End Mock SPIFFS ---


void setUp(void) {
    // Reset the mock filesystem state before each test
    mock_file_content.clear();
    mock_file_exists = false;
    mock_file_read_pos = 0;
}

void tearDown(void) {}

void test_load_creates_default_file_if_not_exists() {
    ConfigManager cm;
    mock_file_exists = false;

    cm.load(); // Should detect file doesn't exist and call save()

    // Verify that the config has default values
    TEST_ASSERT_EQUAL_FLOAT(LOW_DELTA_T_THRESHOLD, cm.getConfig().lowDeltaTThreshold);

    // Verify that save() was called and wrote the default config to the mock file
    JsonDocument doc;
    deserializeJson(doc, mock_file_content);
    TEST_ASSERT_EQUAL_FLOAT(LOW_DELTA_T_THRESHOLD, doc["lowDeltaTThreshold"]);
}

void test_load_parses_existing_file() {
    ConfigManager cm;
    mock_file_exists = true;
    mock_file_content = "{\"lowDeltaTThreshold\":5.5,\"lowDeltaTDurationS\":500,\"noAirflowDurationS\":100,\"tempSensorDisconnectedDurationS\":40}";

    cm.load();

    TEST_ASSERT_EQUAL_FLOAT(5.5f, cm.getConfig().lowDeltaTThreshold);
    TEST_ASSERT_EQUAL_UINT(500, cm.getConfig().lowDeltaTDurationS);
    TEST_ASSERT_EQUAL_UINT(100, cm.getConfig().noAirflowDurationS);
    TEST_ASSERT_EQUAL_UINT(40, cm.getConfig().tempSensorDisconnectedDurationS);
}

void test_save_writes_correct_json() {
    ConfigManager cm;
    cm.getConfig().lowDeltaTThreshold = 9.9f;
    cm.getConfig().lowDeltaTDurationS = 999;

    cm.save();

    JsonDocument doc;
    deserializeJson(doc, mock_file_content);
    TEST_ASSERT_EQUAL_FLOAT(9.9f, doc["lowDeltaTThreshold"]);
    TEST_ASSERT_EQUAL_UINT(999, doc["lowDeltaTDurationS"]);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_load_creates_default_file_if_not_exists);
    RUN_TEST(test_load_parses_existing_file);
    RUN_TEST(test_save_writes_correct_json);
    return UNITY_END();
}