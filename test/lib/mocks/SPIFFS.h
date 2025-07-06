#ifndef MOCK_SPIFFS_H
#define MOCK_SPIFFS_H

// This header provides a mock implementation of the SPIFFS and File classes
// for use in the native test environment. It allows code that depends on the
// filesystem to be tested on the host machine.

#include <string>

class MockFile {
public:
    // Methods required by ArduinoJson for reading
    int read();
    size_t readBytes(char* buffer, size_t length);

    // Methods required by ArduinoJson for writing
    size_t write(uint8_t c);
    size_t write(const uint8_t* buffer, size_t size);

    operator bool() const;
    void close();
};

class MockSPIFFS {
public:
    bool begin(bool formatOnFail = false);
    bool exists(const char* path);
    MockFile open(const char* path, const char* mode);
};

// This global object mimics the real SPIFFS object, allowing the
// ConfigManager to use it without any code changes.
extern MockSPIFFS SPIFFS;

// --- Mock Control Functions ---
// These functions are used by the test suites to control the mock's behavior.
void mock_SPIFFS_set_content(const std::string& content);
std::string mock_SPIFFS_get_content();
void mock_SPIFFS_set_exists(bool exists);
void mock_SPIFFS_reset();

#endif // MOCK_SPIFFS_H