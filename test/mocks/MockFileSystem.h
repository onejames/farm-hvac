#ifndef MOCK_FILE_SYSTEM_H
#define MOCK_FILE_SYSTEM_H

#include "fs/IFileSystem.h"
#include <map>
#include <string>
#include <sstream>
#include <memory>

class MockFile : public IFile {
public:
    MockFile(std::string& content, const std::string& mode) : _content(content), _mode(mode), _valid(true) {
        if (_mode == "r" || _mode == "a") {
            _buffer << _content;
        }
    }

    ~MockFile() override { close(); }

    size_t write(uint8_t c) override {
        if (!_valid || (_mode != "w" && _mode != "a")) return 0;
        _buffer.put(c);
        return 1;
    }

    size_t write(const uint8_t *buf, size_t size) override {
        if (!_valid || (_mode != "w" && _mode != "a")) return 0;
        _buffer.write(reinterpret_cast<const char*>(buf), size);
        return size;
    }

    int read() override { return _buffer.get(); }
    size_t readBytes(char *buffer, size_t length) override {
        _buffer.read(buffer, length);
        return _buffer.gcount();
    }

    void print(const char* content) override { if (_valid && (_mode == "w" || _mode == "a")) _buffer << content; }
    void println(const char* content) override { if (_valid && (_mode == "w" || _mode == "a")) _buffer << content << "\n"; }
    String readString() override { return _buffer.str(); }
    size_t size() override { return _content.size(); }

    void close() override {
        if (_valid && (_mode == "w" || _mode == "a")) {
            _content = _buffer.str();
        }
        _valid = false;
    }

    operator bool() const override { return _valid; }

private:
    std::string& _content;
    std::string _mode;
    bool _valid;
    std::stringstream _buffer;
};

class MockFileSystem : public IFileSystem {
public:
    MockFileSystem() = default;

    bool begin() override { return true; }
    bool exists(const char* path) override { return _fs_data.count(path); }
    bool remove(const char* path) override { return _fs_data.erase(path); }
    bool rename(const char* pathFrom, const char* pathTo) override {
        if (!_fs_data.count(pathFrom)) return false;
        _fs_data[pathTo] = _fs_data[pathFrom];
        _fs_data.erase(pathFrom);
        return true;
    }

    std::unique_ptr<IFile> open(const char* path, const char* mode) override {
        std::string smode(mode);
        if (smode == "r" && !exists(path)) {
            return nullptr;
        }
        // Ensure the file entry exists so MockFile can get a reference to its content.
        return std::make_unique<MockFile>(_fs_data[path], smode);
    }

    // --- Test Helper Methods ---
    void setFileContent(const char* path, const std::string& content) {
        _fs_data[path] = content;
    }

    std::string getFileContent(const char* path) {
        return _fs_data.count(path) ? _fs_data[path] : "";
    }

    void reset() {
        _fs_data.clear();
    }

private:
    std::map<std::string, std::string> _fs_data;
};

#endif // MOCK_FILE_SYSTEM_H