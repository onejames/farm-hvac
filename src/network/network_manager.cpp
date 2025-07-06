#include "network_manager.h"
#include "state/SystemState.h"
#include "logic/json_builder.h"
#include "config/config_manager.h"
#include "config.h"
#include "logging/log_manager.h"
#include "logic/settings_validator.h"
#include <WiFi.h>
#include <Esp.h>
#include <SPIFFS.h>
#include "secrets.h"
#include "version.h"
#include "adapters/pubsub_client_adapter.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>

const long MQTT_RECONNECT_INTERVAL = 5000;

NetworkManager::NetworkManager(SystemState& systemState,
                               ConfigManager& configManager,
                               LogManager& logManager)
    : _systemState(systemState),
      _configManager(configManager),
      _logManager(logManager),
      _client(_net),
      _server(80),
      _lastMqttReconnectAttempt(0) {}

void NetworkManager::setup() {
    setupWiFi();
    setupWebInterface();
}

void NetworkManager::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    _logManager.log("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}

void NetworkManager::setupWebInterface() {
    setupMqtt();
    setupApiRoutes();
    setupStaticFileServer();

    _server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not found");
    });

    _server.begin();
    _logManager.log("HTTP server started");
}

void NetworkManager::setupMqtt() {
    // Configure MQTT client
    _net.setCACert(AWS_CERT_CA);
    _net.setCertificate(AWS_CERT_CRT);
    _net.setPrivateKey(AWS_CERT_PRIVATE);
    _client.setServer(AWS_IOT_ENDPOINT, 8883);
}

void NetworkManager::setupSettingsRoutes() {
    // Route to get current settings
    _server.on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonObject root = response->getRoot();
        const AppConfig& config = _configManager.getConfig();
        root["lowDeltaTThreshold"] = config.lowDeltaTThreshold;
        root["lowDeltaTDurationS"] = config.lowDeltaTDurationS;
        root["noAirflowDurationS"] = config.noAirflowDurationS;
        root["tempSensorDisconnectedDurationS"] = config.tempSensorDisconnectedDurationS;
        response->setLength();
        request->send(response);
    });

    // Route to post new settings
    AsyncCallbackJsonWebHandler* settingsPostHandler = new AsyncCallbackJsonWebHandler("/api/settings", [this](AsyncWebServerRequest *request, JsonVariant &json) {
        JsonObject jsonObj = json.as<JsonObject>();
        
        ValidationResult result = SettingsValidator::validateAndApply(jsonObj, _configManager.getConfig());

        if (result.success) {
            _configManager.save();
            request->send(200, "application/json", "{\"status\":\"ok\", \"message\":\"Settings saved.\"}");
        } else {
            // Use a C-style string for the format to avoid String object creation in the lambda
            char errorPayload[128];
            snprintf(errorPayload, sizeof(errorPayload), "{\"status\":\"error\", \"message\":\"%s\"}", result.message.c_str());
            request->send(400, "application/json", errorPayload);
        }
    });
    settingsPostHandler->setMethod(HTTP_POST);
    _server.addHandler(settingsPostHandler);
}

void NetworkManager::setupSystemRoutes() {
    // Route to trigger a device reboot
    _server.on("/api/reboot", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"ok\", \"message\":\"Rebooting...\"}");
        
        // Add a small delay to ensure the HTTP response is sent before rebooting
        delay(500);
        ESP.restart();
    });

    // Route to trigger a factory reset
    _server.on("/api/factory_reset", HTTP_POST, [this](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"ok\", \"message\":\"Factory reset successful. Rebooting...\"}");
        
        _configManager.remove();
        // Add a small delay to ensure the HTTP response is sent before rebooting
        delay(500);
        ESP.restart();
    });

    // Route to get system logs
    _server.on("/api/logs", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", _logManager.getLogs());
    });

    // Route to clear system logs
    _server.on("/api/logs/clear", HTTP_POST, [this](AsyncWebServerRequest *request) {
        _logManager.clearLogs();
        request->send(200, "application/json", "{\"status\":\"ok\", \"message\":\"Logs cleared.\"}");
    });

    // Route to get device status (uptime, memory)
    _server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonObject root = response->getRoot();
        root["uptime_ms"] = millis();
        root["free_heap_bytes"] = ESP.getFreeHeap();
        response->setLength();
        request->send(response);
    });
}

void NetworkManager::setupApiRoutes() {
    _server.on("/api/data", HTTP_GET, [this](AsyncWebServerRequest *request) {
        char buffer[512];
        JsonBuilder::buildPayload(_systemState.getLatestData(), FIRMWARE_VERSION, BUILD_DATE, buffer, sizeof(buffer));
        request->send(200, "application/json", buffer);
    });

    // Route for the historical data buffer
    _server.on("/api/history", HTTP_GET, [this](AsyncWebServerRequest *request) {
        // Use a dynamic response to handle the larger payload of the history buffer
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonArray root = response->getRoot().to<JsonArray>();
        JsonBuilder::buildHistoryJson(root, _systemState.getDataBuffer(), _systemState.getBufferIndex());
        response->setLength();
        request->send(response);
    });

    // Route for the aggregated historical data buffer
    _server.on("/api/aggregated_history", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonArray root = response->getRoot().to<JsonArray>();
        JsonBuilder::buildAggregatedHistoryJson(root, _systemState.getAggregatedDataBuffer(), _systemState.getAggregatedBufferIndex());
        response->setLength();
        request->send(response);
    });

    setupSettingsRoutes();
    setupSystemRoutes();
}

void NetworkManager::setupStaticFileServer() {
    _server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("index.html")
        .setCacheControl("max-age=3600");
}

void NetworkManager::handleClient() {
    if (!_client.connected()) {
        long now = millis();
        if (now - _lastMqttReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
            _lastMqttReconnectAttempt = now;
            _logManager.log("[MQTT] Attempting to connect to AWS IoT...");
            if (_client.connect(THINGNAME)) {
                _logManager.log("[MQTT] Connected!");
            } else {
                _logManager.log("[MQTT] Connection failed, rc=%d. Retrying in 5 seconds...", _client.state());
            }
        }
    } else {
        _client.loop();
    }
}

void NetworkManager::publishAggregatedData() {
    if (!_client.connected()) {
        return;
    }

    // Get the most recently added aggregated data point.
    // The current index points to the *next* slot to be filled, so we go back one.
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

    PubSubClientAdapter mqttAdapter(_client);
    if (!mqttAdapter.publish(AWS_IOT_TOPIC, reinterpret_cast<const uint8_t*>(payload), payload_size)) {
        _logManager.log("[MQTT] ERROR: Aggregated data publish failed. Message may be too large for MQTT buffer.");
    } else {
        _logManager.log("[MQTT] Published aggregated data.");
    }
}