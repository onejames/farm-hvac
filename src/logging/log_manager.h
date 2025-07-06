#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string>
using String = std::string;
#endif

// Constants used for logging, exposed via `extern` to be accessible for testing.
extern const char* LOG_FILE;
extern const char* OLD_LOG_FILE;
extern const size_t MAX_LOG_SIZE;

class LogManager {
public:
    void begin();
    void log(const char* format, ...);
    String getLogs();
    void clearLogs();

private:
    void rotateLogs();
};

#endif // LOG_MANAGER_H