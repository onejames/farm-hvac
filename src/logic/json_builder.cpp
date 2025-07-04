#include "json_builder.h"
#include "hvac_data.h"
#include <ArduinoJson.h>

size_t JsonBuilder::buildPayload(const HVACData& data, char* buffer, size_t bufferSize) {
  // Per ArduinoJson v7, StaticJsonDocument is deprecated. JsonDocument is preferred.
  JsonDocument doc;

  // Set the values. ArduinoJson will format floats correctly.
  doc["returnTempC"] = data.returnTempC;
  doc["supplyTempC"] = data.supplyTempC;
  doc["deltaT"] = data.deltaT;
  doc["fanAmps"] = data.fanAmps;
  doc["compressorAmps"] = data.compressorAmps;
  doc["geoPumpsAmps"] = data.geoPumpsAmps;
  doc["fanStatus"] = data.fanStatus;
  doc["compressorStatus"] = data.compressorStatus;
  doc["geoPumpsStatus"] = data.geoPumpsStatus;
  doc["airflowStatus"] = data.airflowStatus;
  doc["alertStatus"] = data.alertStatus;

  // Serialize the JSON document to the provided buffer
  return serializeJson(doc, buffer, bufferSize);
}