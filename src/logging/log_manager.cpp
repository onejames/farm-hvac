#include "log_manager.h"
#include "fs/IFileSystem.h"
#ifdef ARDUINO
#include <cstdarg>  // for va_list etc.
#include <cstdio>   // for snprintf, vsnprintf
#endif

const char* LOG_FILE = "/system.log";
const char* OLD_LOG_FILE = "/system.log.old";
const size_t MAX_LOG_SIZE = 4096; // 4KB

LogManager::LogManager(IFileSystem& fs) : _fs(fs) {}

void LogManager::begin() {
    // This is just a placeholder in case any specific setup is needed later.
    // The filesystem is now initialized in the Application class.
}

void LogManager::rotateLogs() {
    if (_fs.exists(OLD_LOG_FILE)) {
        _fs.remove(OLD_LOG_FILE);
    }
    if (_fs.exists(LOG_FILE)) {
        _fs.rename(LOG_FILE, OLD_LOG_FILE);
    }
}

void LogManager::log(const char* format, ...) {
    // --- Format Message ---
    char timestampBuffer[16];
    char messageBuffer[256];

    // Create timestamp string
#ifdef ARDUINO
    snprintf(timestampBuffer, sizeof(timestampBuffer), "[%lu] ", millis());
#else
    snprintf(timestampBuffer, sizeof(timestampBuffer), "[0] "); // Placeholder for native tests
#endif

    // Create formatted message string
    va_list args;
    va_start(args, format);
    vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
    va_end(args);

    // --- Output to Serial ---
#ifdef ARDUINO
    Serial.print(timestampBuffer);
    Serial.println(messageBuffer);
#endif

    // --- Output to File ---
    auto logFile = _fs.open(LOG_FILE, "a");
    if (logFile && logFile->size() > MAX_LOG_SIZE) {
        logFile->close(); // Close before rotating
        rotateLogs();
        logFile = _fs.open(LOG_FILE, "a");
    }

    if (logFile) {
        logFile->print(timestampBuffer);
        logFile->println(messageBuffer);
        logFile->close();
    }
}

String LogManager::getLogs() {
    auto logFile = _fs.open(LOG_FILE, "r");
    if (!logFile) {
        return "No log file found.";
    }
    String logs = logFile->readString();
    logFile->close();
    return logs;
}

void LogManager::clearLogs() {
    _fs.remove(LOG_FILE);
    _fs.remove(OLD_LOG_FILE);
    log("Logs cleared.");
}