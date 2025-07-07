#include "MqttManager.h"
#include "state/SystemState.h"
#include "logging/log_manager.h"
#include "logic/json_builder.h"
#include "network/IPubSubClient.h"
#include "secrets.h"
#include "version.h"
#include "config.h"

#ifndef ARDUINO
#include "mocks/Arduino.h" // For millis() mock in native tests
#endif

const long MQTT_RECONNECT_INTERVAL = 5000;

MqttManager::MqttManager(SystemState& systemState, LogManager& logManager, std::unique_ptr<IPubSubClient> client)
    : _systemState(systemState),
      _logManager(logManager),
      _client(std::move(client)),
      _lastMqttReconnectAttempt(0) {}

// Define the destructor in the .cpp file where IPubSubClient is a complete type.
// This ensures the compiler knows the size and destructor of IPubSubClient
// when destroying the std::unique_ptr.
MqttManager::~MqttManager() = default;

void MqttManager::handleClient() {
    if (!_client) {
        return; // Do nothing if there is no client (e.g., in native tests)
    }

    if (!_client->connected()) {
        long now = millis();
        if (now - _lastMqttReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
            _lastMqttReconnectAttempt = now;
            _logManager.log("[MQTT] Attempting to connect to AWS IoT...");
            if (_client->connect(THINGNAME)) {
                _logManager.log("[MQTT] Connected!");
            } else {
                _logManager.log("[MQTT] Connection failed, rc=%d. Retrying in 5 seconds...", _client->state());
            }
        }
    } else {
        _client->loop();
    }
}

void MqttManager::publishAggregatedData() {
    if (!_client || !_client->connected()) {
        return;
    }

    // Get the most recently added aggregated data point.
    size_t latestIndex = (_systemState.getAggregatedBufferIndex() + AGGREGATED_DATA_BUFFER_SIZE - 1) % AGGREGATED_DATA_BUFFER_SIZE;
    const AggregatedHVACData& dataToPublish = _systemState.getAggregatedDataBuffer()[latestIndex];

    // Don't publish if the entry is uninitialized
    if (dataToPublish.timestamp == 0) {
        return;
    }

    char payload[512];
    size_t payload_size = JsonBuilder::buildPayload(dataToPublish, FIRMWARE_VERSION, BUILD_DATE, payload, sizeof(payload));

    if (payload_size == 0) {
        _logManager.log("[MQTT] ERROR: Aggregated JSON serialization failed.");
        return;
    }

    if (!_client->publish(AWS_IOT_TOPIC, reinterpret_cast<const uint8_t*>(payload), payload_size)) {
        _logManager.log("[MQTT] ERROR: Aggregated data publish failed. Message may be too large for MQTT buffer.");
    } else {
        _logManager.log("[MQTT] Published aggregated data.");
    }
}