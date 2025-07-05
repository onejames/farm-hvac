#include "json_builder.h"
#include <ArduinoJson.h>
#include "hvac_data.h"
#include "enum_converters.h"

void JsonBuilder::serializeHvacDataToJson(JsonObject& doc, const HVACData& data) {
    doc["returnTempC"] = data.returnTempC;
    doc["supplyTempC"] = data.supplyTempC;
    doc["deltaT"] = data.deltaT;
    doc["fanAmps"] = data.fanAmps;
    doc["compressorAmps"] = data.compressorAmps;
    doc["geoPumpsAmps"] = data.geoPumpsAmps;
    doc["fanStatus"] = toString(data.fanStatus);
    doc["compressorStatus"] = toString(data.compressorStatus);
    doc["geoPumpsStatus"] = toString(data.geoPumpsStatus);
    doc["airflowStatus"] = toString(data.airflowStatus);
    doc["alertStatus"] = toString(data.alertStatus);
}

size_t JsonBuilder::buildPayload(const HVACData& data, const char* version, const char* buildDate, char* buffer, size_t bufferSize) {
    // Per ArduinoJson v7, StaticJsonDocument is deprecated. JsonDocument is preferred.
    JsonDocument doc; // Size will be adjusted automatically
    serializeHvacDataToJson(doc.to<JsonObject>(), data);
    doc["version"] = version;
    doc["buildDate"] = buildDate;

    // Serialize the JSON document to the provided buffer
    return serializeJson(doc, buffer, bufferSize);
}

void JsonBuilder::buildHistoryJson(ArduinoJson::JsonArray& history, const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, size_t bufferIndex) {
    // The buffer is circular. The oldest element is at the current index (if the buffer is full).
    // We iterate from the current index, wrap around, and stop before the current index again.
    for (size_t i = 0; i < dataBuffer.size(); ++i) {
        size_t idx = (bufferIndex + i) % dataBuffer.size();
        const HVACData& data = dataBuffer[idx];

        // Don't add empty/default entries if the buffer isn't full yet.
        if (!data.isInitialized) {
            continue;
        }

        JsonObject entry = history.add<JsonObject>();
        serializeHvacDataToJson(entry, data);
    }
}

void JsonBuilder::buildAggregatedHistoryJson(ArduinoJson::JsonArray& history, const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& dataBuffer, size_t bufferIndex) {
    for (size_t i = 0; i < dataBuffer.size(); ++i) {
        size_t idx = (bufferIndex + i) % dataBuffer.size();
        const AggregatedHVACData& data = dataBuffer[idx];

        // Skip uninitialized entries
        if (data.timestamp == 0) {
            continue;
        }

        JsonObject entry = history.add<JsonObject>();
        entry["timestamp"] = data.timestamp;
        entry["avgReturnTempC"] = data.avgReturnTempC;
        entry["avgSupplyTempC"] = data.avgSupplyTempC;
        entry["avgDeltaT"] = data.avgDeltaT;
        entry["avgFanAmps"] = data.avgFanAmps;
        entry["avgCompressorAmps"] = data.avgCompressorAmps;
        entry["avgGeoPumpsAmps"] = data.avgGeoPumpsAmps;
        entry["lastFanStatus"] = toString(data.lastFanStatus);
        entry["lastCompressorStatus"] = toString(data.lastCompressorStatus);
        entry["lastGeoPumpsStatus"] = toString(data.lastGeoPumpsStatus);
    }
}

size_t JsonBuilder::buildPayload(const AggregatedHVACData& data, const char* version, const char* buildDate, char* buffer, size_t bufferSize) {
    JsonDocument doc;
    doc["timestamp"] = data.timestamp;
    doc["avgReturnTempC"] = data.avgReturnTempC;
    doc["avgSupplyTempC"] = data.avgSupplyTempC;
    doc["avgDeltaT"] = data.avgDeltaT;
    doc["avgFanAmps"] = data.avgFanAmps;
    doc["avgCompressorAmps"] = data.avgCompressorAmps;
    doc["avgGeoPumpsAmps"] = data.avgGeoPumpsAmps;
    doc["lastFanStatus"] = toString(data.lastFanStatus);
    doc["lastCompressorStatus"] = toString(data.lastCompressorStatus);
    doc["lastGeoPumpsStatus"] = toString(data.lastGeoPumpsStatus);
    doc["version"] = version;
    doc["buildDate"] = buildDate;

    return serializeJson(doc, buffer, bufferSize);
}