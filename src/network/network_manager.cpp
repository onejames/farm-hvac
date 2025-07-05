#include "network_manager.h"
#include "logic/json_builder.h"
#include "config.h"

#ifdef ARDUINO
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <PubSubClient.h>
#include "secrets.h"
#include "version.h"
#include "adapters/pubsub_client_adapter.h"

// Global objects declared in main.cpp
extern AsyncWebServer server;
extern HVACData hvacData;
extern PubSubClient client;

// MQTT Reconnect logic
const long mqttReconnectInterval = 5000;
unsigned long lastMqttReconnectAttempt = 0;

void setupWiFi() {
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

void setupWebInterface() {
    if(!SPIFFS.begin(true)){
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    // Route for the JSON data API. Accesses the global hvacData struct.
    server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request) {
        char buffer[512];
        JsonBuilder::buildPayload(hvacData, FIRMWARE_VERSION, BUILD_DATE, buffer, sizeof(buffer));
        request->send(200, "application/json", buffer);
    });

    // Serve static web interface files from SPIFFS root
    server.serveStatic("/", SPIFFS, "/")
        .setDefaultFile("index.html")
        .setCacheControl("max-age=3600");

    server.onNotFound([](AsyncWebServerRequest *request){
        request->send(404, "text/plain", "Not found");
    });

    server.begin();
    Serial.println("HTTP server started");
}

void handleMqttClient() {
    if (!client.connected()) {
        long now = millis();
        if (now - lastMqttReconnectAttempt > mqttReconnectInterval) {
            lastMqttReconnectAttempt = now;
            Serial.print("[MQTT] Attempting to connect to AWS IoT...");
            if (client.connect(THINGNAME)) {
                Serial.println(" Connected!");
            } else {
                Serial.print(" failed, rc=");
                Serial.print(client.state());
                Serial.println(". Retrying in 5 seconds...");
            }
        }
    } else {
        client.loop();
    }
}

void publishMessage() {
    PubSubClientAdapter mqttAdapter(client);
    publishMqttMessage(mqttAdapter, AWS_IOT_TOPIC, hvacData, FIRMWARE_VERSION, BUILD_DATE);
}
#endif // ARDUINO

bool publishMqttMessage(IMqttClient& client, const char* topic, const HVACData& data, const char* version, const char* buildDate) {
    if (!client.connected()) {
        return false;
    }

    char payload[512];
    size_t payload_size = JsonBuilder::buildPayload(data, version, buildDate, payload, sizeof(payload));

    if (payload_size == 0) {
#ifdef ARDUINO
        Serial.println("[MQTT] JSON serialization failed.");
#endif
        return false;
    }

    if (!client.publish(topic, reinterpret_cast<const uint8_t*>(payload), payload_size)) {
#ifdef ARDUINO
        Serial.println("[MQTT] Publish failed. Message may be too large for MQTT buffer.");
#endif
        return false;
    }
    return true;
}