#include "json_builder.h"
#include <ArduinoJson.h>
#include "hvac_data.h"
#include "enum_converters.h"

size_t JsonBuilder::buildPayload(const HVACData& data, const char* version, const char* buildDate, char* buffer, size_t bufferSize) {
    // Per ArduinoJson v7, StaticJsonDocument is deprecated. JsonDocument is preferred.
    JsonDocument doc; // Size will be adjusted automatically

    // Set the values. ArduinoJson will format floats correctly.
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
    doc["version"] = version;
    doc["buildDate"] = buildDate;

    // Serialize the JSON document to the provided buffer
    return serializeJson(doc, buffer, bufferSize);
}