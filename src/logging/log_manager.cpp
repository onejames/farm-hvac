#include "log_manager.h"
#ifdef ARDUINO
#include <SPIFFS.h>
#include <Arduino.h> // For Serial, millis()
#else
#include <SPIFFS.h> // Uses the mock version
#include <cstdarg>  // for va_list etc.
#include <cstdio>   // for snprintf, vsnprintf
#endif

const char* LOG_FILE = "/system.log";
const char* OLD_LOG_FILE = "/system.log.old";
const size_t MAX_LOG_SIZE = 4096; // 4KB

void LogManager::begin() {
    // This is just a placeholder in case any specific setup is needed later.
    // SPIFFS is initialized in the NetworkManager.
}

void LogManager::rotateLogs() {
    if (SPIFFS.exists(OLD_LOG_FILE)) {
        SPIFFS.remove(OLD_LOG_FILE);
    }
    if (SPIFFS.exists(LOG_FILE)) {
        SPIFFS.rename(LOG_FILE, OLD_LOG_FILE);
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
    auto logFile = SPIFFS.open(LOG_FILE, "a");
    if (logFile && logFile.size() > MAX_LOG_SIZE) {
        logFile.close(); // Close before rotating
        rotateLogs();
        logFile = SPIFFS.open(LOG_FILE, "a");
    }

    if (logFile) {
        logFile.print(timestampBuffer);
        logFile.println(messageBuffer);
        logFile.close();
    }
}

String LogManager::getLogs() {
    auto logFile = SPIFFS.open(LOG_FILE, "r");
    if (!logFile) {
        return "No log file found.";
    }
    String logs = logFile.readString();
    logFile.close();
    return logs;
}

void LogManager::clearLogs() {
    if (SPIFFS.exists(LOG_FILE)) {
        SPIFFS.remove(LOG_FILE);
    }
    if (SPIFFS.exists(OLD_LOG_FILE)) {
        SPIFFS.remove(OLD_LOG_FILE);
    }
    log("Logs cleared.");
}