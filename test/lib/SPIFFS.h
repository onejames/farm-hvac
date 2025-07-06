#ifndef MOCK_SPIFFS_H
#define MOCK_SPIFFS_H

#include <string>
#include <map>
#include <vector>
#include <sstream>

// In the native test environment, we can use std::string for the Arduino String class.
using String = std::string;

// --- Mock Internals & Test Helpers ---
// This map simulates the filesystem, mapping file paths to their content.
// Declared inline to ensure a single definition is shared across all test units.
inline std::map<std::string, std::string> mock_fs;

inline void mock_SPIFFS_reset() {
    mock_fs.clear();
}

inline void mock_SPIFFS_set_file_content(const char* path, std::string content) {
    mock_fs[path] = content;
}

inline std::string mock_SPIFFS_get_file_content(const char* path) {
    if (mock_fs.count(path)) {
        return mock_fs[path];
    }
    return "";
}

// --- Mock File Class ---
// This class mimics the behavior of the File object returned by SPIFFS.open().
class MockFile {
private:
    std::string _path;
    std::string _mode;
    bool _valid;
    std::stringstream _buffer; // Used to handle appends before the file is "closed".

public:
    MockFile() : _valid(false) {}

    MockFile(const std::string& path, const std::string& mode) : _path(path), _mode(mode), _valid(true) {
        // If opening an existing file for reading or appending, load its content into the buffer.
        if (_mode == "a" || _mode == "r") {
            if (mock_fs.count(_path)) {
                _buffer << mock_fs[_path];
            }
        }
    }

    // Returns the size of the file as it exists on the mock filesystem.
    size_t size() {
        if (!_valid) return 0;
        // For log rotation, size() should reflect the size *before* opening for append.
        return mock_fs.count(_path) ? mock_fs[_path].size() : 0;
    }

    // Appends content to the internal buffer if open in append mode.
    void print(const char* content) {
        if (_valid && (_mode == "a" || _mode == "w")) {
            _buffer << content;
        }
    }

    void println(const char* content) {
        if (_valid && (_mode == "a" || _mode == "w")) {
            _buffer << content << "\n";
        }
    }

    // Methods required by ArduinoJson for writing. These mimic the Arduino Print interface.
    size_t write(uint8_t c) {
        if (_valid && (_mode == "a" || _mode == "w")) {
            _buffer.put(c);
            return 1;
        }
        return 0;
    }

    size_t write(const uint8_t* buffer, size_t size) {
        if (!_valid || (_mode != "a" && _mode != "w")) return 0;
        _buffer.write(reinterpret_cast<const char*>(buffer), size);
        return size;
    }

    // Methods required by ArduinoJson for reading. These mimic the Arduino Stream interface.
    int read() {
        if (!_valid || (_mode != "r" && _mode != "a")) {
            return -1;
        }
        return _buffer.get(); // Returns the character or EOF (-1)
    }

    size_t readBytes(char* buffer, size_t length) {
        if (!_valid || (_mode != "r" && _mode != "a")) {
            return 0;
        }
        _buffer.read(buffer, length);
        return _buffer.gcount(); // Returns the number of characters actually read
    }

    // Returns the file's content.
    String readString() {
        return (_valid && _mode == "r") ? _buffer.str() : "";
    }

    // "Closes" the file. If in append mode, writes the buffer back to the mock filesystem.
    void close() {
        // Also save on "w" (write) mode, which is used by ConfigManager::save()
        if (_valid && (_mode == "a" || _mode == "w")) {
            mock_fs[_path] = _buffer.str();
        }
        _valid = false;
    }

    // Allows checking the file's validity with `if (file)`.
    operator bool() const {
        return _valid;
    }
};

// --- Mock SPIFFS Class ---
class MockSPIFFS {
public:
    bool begin(bool formatOnFail = false) { return true; } // Mock begin always succeeds.
    bool exists(const char* path) { return mock_fs.count(path); }
    void remove(const char* path) { mock_fs.erase(path); }
    bool rename(const char* pathFrom, const char* pathTo) {
        if (!mock_fs.count(pathFrom)) return false;
        mock_fs[pathTo] = mock_fs[pathFrom];
        mock_fs.erase(pathFrom);
        return true;
    }
    MockFile open(const char* path, const char* mode) {
        // If opening for read and file doesn't exist, return an invalid file object.
        return (std::string(mode) == "r" && !exists(path)) ? MockFile() : MockFile(path, mode);
    }
};

// A global instance to mimic Arduino's `SPIFFS` object.
inline MockSPIFFS SPIFFS;

#endif // MOCK_SPIFFS_H