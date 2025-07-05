#include "network_manager.h"
#include "logic/json_builder.h"
#include "config/config_manager.h"
#include "config.h"
#include "logic/settings_validator.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include "secrets.h"
#include "version.h"
#include "adapters/pubsub_client_adapter.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>

const long MQTT_RECONNECT_INTERVAL = 5000;

NetworkManager::NetworkManager(HVACData& latestData,
                               const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const size_t& bufferIndex,
                               const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& aggregatedBuffer, const size_t& aggregatedBufferIndex,
                               ConfigManager& configManager)
    : _hvacData(latestData),
      _dataBuffer(dataBuffer),
      _bufferIndex(bufferIndex),
      _aggregatedDataBuffer(aggregatedBuffer),
      _aggregatedBufferIndex(aggregatedBufferIndex),
      _configManager(configManager),
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
    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void NetworkManager::setupWebInterface() {
    // Configure MQTT client
    _net.setCACert(AWS_CERT_CA);
    _net.setCertificate(AWS_CERT_CRT);
    _net.setPrivateKey(AWS_CERT_PRIVATE);
    _client.setServer(AWS_IOT_ENDPOINT, 8883);

    // Setup the web server routes
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    // Route for the JSON data API. Accesses the global hvacData struct.
    _server.on("/api/data", HTTP_GET, [this](AsyncWebServerRequest *request) {
        char buffer[512];
        JsonBuilder::buildPayload(_hvacData, FIRMWARE_VERSION, BUILD_DATE, buffer, sizeof(buffer));
        request->send(200, "application/json", buffer);
    });

    // Route for the historical data buffer
    _server.on("/api/history", HTTP_GET, [this](AsyncWebServerRequest *request) {
        // Use a dynamic response to handle the larger payload of the history buffer
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonArray root = response->getRoot().to<JsonArray>();
        JsonBuilder::buildHistoryJson(root, _dataBuffer, _bufferIndex);
        response->setLength();
        request->send(response);
    });

    // Route for the aggregated historical data buffer
    _server.on("/api/aggregated_history", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonArray root = response->getRoot().to<JsonArray>();
        JsonBuilder::buildAggregatedHistoryJson(root, _aggregatedDataBuffer, _aggregatedBufferIndex);
        response->setLength();
        request->send(response);
    });

    // Route to get current settings
    _server.on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncJsonResponse * response = new AsyncJsonResponse();
        JsonObject root = response->getRoot();
        const AppConfig& config = _configManager.getConfig();
        root["lowDeltaTThreshold"] = config.lowDeltaTThreshold;
        root["lowDeltaTDurationS"] = config.lowDeltaTDurationS;
        root["noAirflowDurationS"] = config.noAirflowDurationS;
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

    // Serve static web interface files from SPIFFS root
    _server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("index.html")
        .setCacheControl("max-age=3600");

    _server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not found");
    });

    _server.begin();
    Serial.println("HTTP server started");
}

void NetworkManager::handleClient() {
    if (!_client.connected()) {
        long now = millis();
        if (now - _lastMqttReconnectAttempt > MQTT_RECONNECT_INTERVAL) {
            _lastMqttReconnectAttempt = now;
            Serial.print("[MQTT] Attempting to connect to AWS IoT...");
            if (_client.connect(THINGNAME)) {
                Serial.println(" Connected!");
            } else {
                Serial.print(" failed, rc=");
                Serial.print(_client.state());
                Serial.println(". Retrying in 5 seconds...");
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
    size_t latestIndex = (_aggregatedBufferIndex + AGGREGATED_DATA_BUFFER_SIZE - 1) % AGGREGATED_DATA_BUFFER_SIZE;
    const AggregatedHVACData& dataToPublish = _aggregatedDataBuffer[latestIndex];

    // Don't publish if the entry is uninitialized
    if (dataToPublish.timestamp == 0) {
        return;
    }

    char payload[512];
    size_t payload_size = JsonBuilder::buildPayload(dataToPublish, FIRMWARE_VERSION, BUILD_DATE, payload, sizeof(payload));

    if (payload_size == 0) {
        Serial.println("[MQTT] Aggregated JSON serialization failed.");
        return;
    }

    PubSubClientAdapter mqttAdapter(_client);
    if (!mqttAdapter.publish(AWS_IOT_TOPIC, reinterpret_cast<const uint8_t*>(payload), payload_size)) {
        Serial.println("[MQTT] Aggregated data publish failed. Message may be too large for MQTT buffer.");
    } else {
        Serial.println("[MQTT] Published aggregated data.");
    }
}