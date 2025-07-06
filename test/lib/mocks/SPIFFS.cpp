// This file provides the implementation for the mock SPIFFS and File classes
// used in the native test environment. It is only compiled for native tests.

#include "SPIFFS.h"
#include <cstring> // For memcpy, strcmp
#include <algorithm> // For std::min

static std::string mock_file_content;
static bool mock_file_exists = false;
static size_t mock_file_read_pos = 0;

// --- MockFile Implementation ---

int MockFile::read() {
    if (mock_file_read_pos < mock_file_content.length()) {
        return mock_file_content[mock_file_read_pos++];
    }
    return -1; // End of file
}

size_t MockFile::readBytes(char* buffer, size_t length) {
    size_t bytes_to_read = 0;
    if (mock_file_read_pos < mock_file_content.length()) {
        bytes_to_read = std::min(length, mock_file_content.length() - mock_file_read_pos);
        memcpy(buffer, mock_file_content.c_str() + mock_file_read_pos, bytes_to_read);
        mock_file_read_pos += bytes_to_read;
    }
    return bytes_to_read;
}

size_t MockFile::write(uint8_t c) {
    mock_file_content += static_cast<char>(c);
    return 1;
}

size_t MockFile::write(const uint8_t* buffer, size_t size) {
    mock_file_content.append(reinterpret_cast<const char*>(buffer), size);
    return size;
}

MockFile::operator bool() const { return true; }
void MockFile::close() {}

// --- MockSPIFFS Implementation ---

bool MockSPIFFS::begin(bool formatOnFail) { return true; }
bool MockSPIFFS::exists(const char* path) { return mock_file_exists; }
bool MockSPIFFS::remove(const char* path) {
    if (strcmp(path, "/config.json") == 0) {
        mock_file_exists = false;
        mock_file_content.clear();
    }
    return true;
}
MockFile MockSPIFFS::open(const char* path, const char* mode) {
    if (strcmp(mode, "w") == 0) {
        mock_file_content.clear();
    }
    mock_file_read_pos = 0;
    return MockFile();
}

MockSPIFFS SPIFFS;

// --- Mock Control Functions ---
void mock_SPIFFS_set_content(const std::string& content) { mock_file_content = content; }
std::string mock_SPIFFS_get_content() { return mock_file_content; }
void mock_SPIFFS_set_exists(bool exists) { mock_file_exists = exists; }
void mock_SPIFFS_reset() {
    mock_file_content.clear();
    mock_file_exists = false;
    mock_file_read_pos = 0;
}