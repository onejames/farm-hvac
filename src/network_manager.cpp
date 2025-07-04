#include "network_manager.h"
#include "secrets.h"
#include "hvac_data.h"

// Required libraries for this module
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const long mqttReconnectInterval = 5000; // Attempt reconnect every 5 seconds

NetworkManager::NetworkManager(WiFiClientSecure& net, PubSubClient& client, AsyncWebServer& server)
    : _net(net),
      _client(client),
      _server(server),
      _lastMqttReconnectAttempt(0) {}

void NetworkManager::setup(HVACData& data) {
  // Connect to Wi-Fi (blocking, intended for setup())
  Serial.print("[WIFI] Connecting to ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  Serial.print("[WIFI] IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure WiFiClientSecure to use the AWS certificates
  _net.setCACert(AWS_CERT_CA);
  _net.setCertificate(AWS_CERT_CRT);
  _net.setPrivateKey(AWS_CERT_PRIVATE);

  // Configure the MQTT client
  _client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Setup the web server
  _server.on("/", HTTP_GET, [&data](AsyncWebServerRequest *request){
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>HVAC Monitor</title>");
    response->print("<meta http-equiv='refresh' content='5'></head><body>");
    response->printf("<h1>HVAC Real-Time Status</h1><h2>Temperatures</h2><p>Return Air: %.1f &deg;C</p>", data.returnTempC);
    response->printf("<p>Supply Air: %.1f &deg;C</p><p><b>Delta T: %.1f &deg;C</b></p>", data.supplyTempC, data.deltaT);
    response->print("<h2>Component Status</h2>");
    response->printf("<p>Fan: %s (%.2f A)</p>", data.fanStatus.c_str(), data.fanAmps);
    response->printf("<p>Compressor: %s (%.2f A)</p>", data.compressorStatus.c_str(), data.compressorAmps);
    response->printf("<p>Geothermal Pumps: %s (%.2f A)</p><h2>System</h2>", data.geoPumpsStatus.c_str(), data.geoPumpsAmps);
    response->printf("<p>Airflow: %s</p><p><b>Alerts: %s</b></p></body></html>", data.airflowStatus.c_str(), data.alertStatus.c_str());
    request->send(response);
  });
  _server.begin();
  Serial.println("[SETUP] Web server started.");
}

void NetworkManager::publish(const HVACData& data) {
  if (!_client.connected()) {
    // Silently return if not connected. The handleMqttClient function will show reconnection messages.
    return;
  }

  // Use ArduinoJson to create a safe and efficient payload.
  // The size (256) is an estimate. Adjust if you add more fields.
  StaticJsonDocument<256> doc;

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

  // Serialize the JSON document to a char buffer
  char payload[256];
  size_t n = serializeJson(doc, payload);

  if (!_client.publish(AWS_IOT_TOPIC, payload, n)) {
      Serial.println("[MQTT] Publish failed. Message may be too large for MQTT buffer.");
  }
}

void NetworkManager::handleClient() {
  if (!_client.connected()) {
    long now = millis();
    if (now - _lastMqttReconnectAttempt > mqttReconnectInterval) {
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