#include <unity.h>
#include "logging/log_manager.h"
#include "mocks/MockFileSystem.h"

void setUp(void) {}

void tearDown(void) {}

void test_log_writes_to_file() {
    MockFileSystem mockFS;
    LogManager lm(mockFS);

    lm.log("Test message %d", 123);

    std::string content = mockFS.getFileContent("/system.log");
    TEST_ASSERT_TRUE(content.find("Test message 123") != std::string::npos);
    TEST_ASSERT_TRUE(content.find("[") == 0); // Starts with a timestamp
}

void test_getLogs_reads_file_content() {
    MockFileSystem mockFS;
    LogManager lm(mockFS);
    mockFS.setFileContent("/system.log", "Hello Log");

    String logs = lm.getLogs();

    TEST_ASSERT_EQUAL_STRING("Hello Log", logs.c_str());
}

void test_clearLogs_removes_files() {
    MockFileSystem mockFS;
    LogManager lm(mockFS);
    mockFS.setFileContent("/system.log", "data");
    mockFS.setFileContent("/system.log.old", "old_data");

    lm.clearLogs();

    // The clearLogs function also logs a message, so a new file is created.
    TEST_ASSERT_TRUE(mockFS.exists("/system.log"));
    TEST_ASSERT_FALSE(mockFS.exists("/system.log.old"));
    TEST_ASSERT_TRUE(mockFS.getFileContent("/system.log").find("Logs cleared") != std::string::npos);
}

void test_log_rotation_works_correctly() {
    MockFileSystem mockFS;
    LogManager lm(mockFS);

    // Create a log content that is already over the max size to guarantee rotation.
    // The check is `size > MAX_LOG_SIZE`, so we make it slightly larger.
    std::string large_content(MAX_LOG_SIZE + 1, 'A');
    mockFS.setFileContent("/system.log", large_content);

    // This log should trigger rotation
    lm.log("This is the new log entry.");

    // Check that the old log file was created and has the old content
    TEST_ASSERT_TRUE(mockFS.exists("/system.log.old"));
    std::string old_content = mockFS.getFileContent("/system.log.old");
    TEST_ASSERT_EQUAL_STRING(large_content.c_str(), old_content.c_str());

    // Check that the new log file has only the new entry
    std::string new_content = mockFS.getFileContent("/system.log");
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