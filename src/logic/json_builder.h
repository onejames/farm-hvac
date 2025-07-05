#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include <cstddef> // for size_t
#include "config.h"
#include <array>   // for std::array
#include <ArduinoJson.h>

// Forward declaration
struct HVACData;
struct AggregatedHVACData;

class JsonBuilder {
public:
    // Returns the number of bytes written to the buffer.
    static size_t buildPayload(const HVACData& data, const char* version, const char* buildDate, char* buffer, size_t bufferSize);

    // Populates a JsonArray with historical data from the circular buffer.
    static void buildHistoryJson(ArduinoJson::JsonArray& history, const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, size_t bufferIndex);

    // Populates a JsonArray with aggregated historical data.
    static void buildAggregatedHistoryJson(ArduinoJson::JsonArray& history, const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& dataBuffer, size_t bufferIndex);

    // Overload for aggregated data payload
    static size_t buildPayload(const AggregatedHVACData& data, const char* version, const char* buildDate, char* buffer, size_t bufferSize);
};

#endif // JSON_BUILDER_H