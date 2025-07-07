#include "SPIFFSFileSystem.h"

#ifdef ARDUINO
// --- SPIFFSFile ---
SPIFFSFile::SPIFFSFile(File file) : _file(file) {}
SPIFFSFile::~SPIFFSFile() {
    if (_file) {
        _file.close();
    }
}
size_t SPIFFSFile::write(uint8_t c) { return _file.write(c); }
size_t SPIFFSFile::write(const uint8_t *buf, size_t size) { return _file.write(buf, size); }
int SPIFFSFile::read() { return _file.read(); }
size_t SPIFFSFile::readBytes(char *buffer, size_t length) { return _file.readBytes(buffer, length); }
void SPIFFSFile::print(const char* content) { _file.print(content); }
void SPIFFSFile::println(const char* content) { _file.println(content); }
String SPIFFSFile::readString() { return _file.readString(); }
size_t SPIFFSFile::size() { return _file.size(); }
void SPIFFSFile::close() { _file.close(); }
SPIFFSFile::operator bool() const { return !!_file; }

// --- SPIFFSFileSystem ---
SPIFFSFileSystem::SPIFFSFileSystem() {}
SPIFFSFileSystem::~SPIFFSFileSystem() {}

bool SPIFFSFileSystem::begin() {
    return SPIFFS.begin(true);
}

bool SPIFFSFileSystem::exists(const char* path) {
    return SPIFFS.exists(path);
}

bool SPIFFSFileSystem::remove(const char* path) {
    return SPIFFS.remove(path);
}

bool SPIFFSFileSystem::rename(const char* pathFrom, const char* pathTo) {
    return SPIFFS.rename(pathFrom, pathTo);
}

std::unique_ptr<IFile> SPIFFSFileSystem::open(const char* path, const char* mode) {
    File file = SPIFFS.open(path, mode);
    if (!file) {
        return nullptr;
    }
    return std::unique_ptr<IFile>(new SPIFFSFile(file));
}
#else
// Native build "hollow" implementations

// --- SPIFFSFile ---
SPIFFSFile::~SPIFFSFile() {}
size_t SPIFFSFile::write(uint8_t) { return 0; }
size_t SPIFFSFile::write(const uint8_t*, size_t) { return 0; }
int SPIFFSFile::read() { return -1; }
size_t SPIFFSFile::readBytes(char*, size_t) { return 0; }
void SPIFFSFile::print(const char*) {}
void SPIFFSFile::println(const char*) {}
String SPIFFSFile::readString() { return String(); }
size_t SPIFFSFile::size() { return 0; }
void SPIFFSFile::close() {}
SPIFFSFile::operator bool() const { return false; }

// --- SPIFFSFileSystem ---
SPIFFSFileSystem::SPIFFSFileSystem() {}
SPIFFSFileSystem::~SPIFFSFileSystem() {}
bool SPIFFSFileSystem::begin() { return true; }
bool SPIFFSFileSystem::exists(const char*) { return false; }
bool SPIFFSFileSystem::remove(const char*) { return false; }
bool SPIFFSFileSystem::rename(const char*, const char*) { return false; }
std::unique_ptr<IFile> SPIFFSFileSystem::open(const char*, const char*) {
    // Return a new, empty (and invalid) file object
    return std::make_unique<SPIFFSFile>();
}

#endif