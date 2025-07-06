#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <string>
#ifdef ARDUINO
#include <Arduino.h>
#else
using String = std::string;
#endif

// Constants used for logging, exposed via `extern` to be accessible for testing.
extern const char* LOG_FILE;
extern const char* OLD_LOG_FILE;
extern const size_t MAX_LOG_SIZE;

class IFileSystem; // Forward declaration

class LogManager {
public:
    explicit LogManager(IFileSystem& fs);
    void begin(); // Kept for potential future use, though currently empty.
    void log(const char* format, ...);
    [[nodiscard]] String getLogs();
    void clearLogs();

private:
    IFileSystem& _fs;
    void rotateLogs();
};

#endif // LOG_MANAGER_H