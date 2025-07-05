#include "network_manager.h"
#include "logic/json_builder.h"
#include "config.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include "secrets.h"
#include "version.h"
#include "adapters/pubsub_client_adapter.h"

const long MQTT_RECONNECT_INTERVAL = 5000;

NetworkManager::NetworkManager(HVACData& data)
    : _hvacData(data),
      _client(_net),
      _server(80),
      _lastMqttReconnectAttempt(0),
      _lastPublishTime(0) {}

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

void NetworkManager::publish() {
    unsigned long currentTime = millis();
    if (currentTime - _lastPublishTime < SENSOR_READ_INTERVAL_MS) {
        return; // Not time to publish yet
    }
    _lastPublishTime = currentTime;

    PubSubClientAdapter mqttAdapter(_client);
    publishMqttMessage(mqttAdapter, AWS_IOT_TOPIC, FIRMWARE_VERSION, BUILD_DATE);
}

bool NetworkManager::publishMqttMessage(IMqttClient& client, const char* topic, const char* version, const char* buildDate) {
    if (!client.connected()) {
        return false;
    }

    char payload[512];
    size_t payload_size = JsonBuilder::buildPayload(_hvacData, version, buildDate, payload, sizeof(payload));

    if (payload_size == 0) {
        Serial.println("[MQTT] JSON serialization failed.");
        return false;
    }

    if (!client.publish(topic, reinterpret_cast<const uint8_t*>(payload), payload_size)) {
        Serial.println("[MQTT] Publish failed. Message may be too large for MQTT buffer.");
        return false;
    }
    return true;
}