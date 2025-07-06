#include <unity.h>
#include "logging/log_manager.h"
#include <SPIFFS.h> // Uses the mock version

void setUp(void) {
    mock_SPIFFS_reset();
}

void tearDown(void) {}

void test_log_writes_to_file() {
    LogManager lm;
    lm.begin();

    lm.log("Test message %d", 123);

    std::string content = mock_SPIFFS_get_file_content("/system.log");
    TEST_ASSERT_TRUE(content.find("Test message 123") != std::string::npos);
    TEST_ASSERT_TRUE(content.find("[") == 0); // Starts with a timestamp
}

void test_getLogs_reads_file_content() {
    LogManager lm;
    lm.begin();
    mock_SPIFFS_set_file_content("/system.log", "Hello Log");

    String logs = lm.getLogs();

    TEST_ASSERT_EQUAL_STRING("Hello Log", logs.c_str());
}

void test_clearLogs_removes_files() {
    LogManager lm;
    lm.begin();
    mock_SPIFFS_set_file_content("/system.log", "data");
    mock_SPIFFS_set_file_content("/system.log.old", "old_data");

    lm.clearLogs();

    // The clearLogs function also logs a message, so a new file is created.
    TEST_ASSERT_TRUE(SPIFFS.exists("/system.log"));
    TEST_ASSERT_FALSE(SPIFFS.exists("/system.log.old"));
    TEST_ASSERT_TRUE(mock_SPIFFS_get_file_content("/system.log").find("Logs cleared") != std::string::npos);
}

void test_log_rotation_works_correctly() {
    LogManager lm;
    lm.begin();

    // Create a log content that is already over the max size to guarantee rotation.
    // The check is `size > MAX_LOG_SIZE`, so we make it slightly larger.
    std::string large_content(MAX_LOG_SIZE + 1, 'A');
    mock_SPIFFS_set_file_content("/system.log", large_content);

    // This log should trigger rotation
    lm.log("This is the new log entry.");

    // Check that the old log file was created and has the old content
    TEST_ASSERT_TRUE(SPIFFS.exists("/system.log.old"));
    std::string old_content = mock_SPIFFS_get_file_content("/system.log.old");
    TEST_ASSERT_EQUAL_STRING(large_content.c_str(), old_content.c_str());

    // Check that the new log file has only the new entry
    std::string new_content = mock_SPIFFS_get_file_content("/system.log");
    TEST_ASSERT_TRUE(new_content.find("This is the new log entry.") != std::string::npos);
    TEST_ASSERT_TRUE(new_content.length() < 100); // Should be small
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_log_writes_to_file);
    RUN_TEST(test_getLogs_reads_file_content);
    RUN_TEST(test_clearLogs_removes_files);
    RUN_TEST(test_log_rotation_works_correctly);
    return UNITY_END();
}