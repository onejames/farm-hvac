#ifndef I_FILE_SYSTEM_H
#define I_FILE_SYSTEM_H

#include <memory> // for std::unique_ptr

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
using String = std::string;
#endif

// Interface for a file object, abstracting away the concrete File implementation.
class IFile {
public:
    virtual ~IFile() = default;
    virtual size_t write(uint8_t) = 0;
    virtual size_t write(const uint8_t *buf, size_t size) = 0;
    virtual int read() = 0;
    virtual size_t readBytes(char *buffer, size_t length) = 0;
    virtual void print(const char* content) = 0;
    virtual void println(const char* content) = 0;
    virtual String readString() = 0;
    virtual size_t size() = 0;
    virtual void close() = 0;
    virtual operator bool() const = 0;
};

// Interface for a filesystem, abstracting away the concrete SPIFFS implementation.
class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    [[nodiscard]] virtual bool begin() = 0;
    [[nodiscard]] virtual bool exists(const char* path) = 0;
    virtual bool remove(const char* path) = 0;
    virtual bool rename(const char* pathFrom, const char* pathTo) = 0;
    [[nodiscard]] virtual std::unique_ptr<IFile> open(const char* path, const char* mode) = 0;
};

#endif // I_FILE_SYSTEM_H