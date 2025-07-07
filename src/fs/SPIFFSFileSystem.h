#ifndef SPIFFS_FILE_SYSTEM_H
#define SPIFFS_FILE_SYSTEM_H

#include "IFileSystem.h"
#ifdef ARDUINO
#include <SPIFFS.h>
#endif

class SPIFFSFile : public IFile {
public:
#ifdef ARDUINO
    explicit SPIFFSFile(File file);
#else
    SPIFFSFile() = default;
#endif
    ~SPIFFSFile() override;

    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int read() override;
    size_t readBytes(char *buffer, size_t length) override;
    void print(const char* content) override;
    void println(const char* content) override;
    String readString() override;
    size_t size() override;
    void close() override;
    operator bool() const override;

private:
#ifdef ARDUINO
    File _file;
#endif
};

class SPIFFSFileSystem : public IFileSystem {
public:
    SPIFFSFileSystem();
    ~SPIFFSFileSystem() override;

    bool begin() override;
    bool exists(const char* path) override;
    bool remove(const char* path) override;
    bool rename(const char* pathFrom, const char* pathTo) override;
    std::unique_ptr<IFile> open(const char* path, const char* mode) override;
};

#endif // SPIFFS_FILE_SYSTEM_H