#include "network_manager.h"
#include "hvac_data.h"
#include "interfaces/i_mqtt_client.h"
#include "logic/json_builder.h"
#include "logic/html_builder.h"

// Required libraries for this module
#ifdef ARDUINO
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include "version.h" // For FIRMWARE_VERSION

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
  _server.on("/", HTTP_GET, [&data](AsyncWebServerRequest *request) {
    String html = HtmlBuilder::build(data, FIRMWARE_VERSION);
    request->send(200, "text/html", html);
  });
  _server.begin();
  Serial.println("[SETUP] Web server started.");
}
#endif // ARDUINO

bool NetworkManager::publish(IMqttClient& client, const char* topic, const HVACData& data) {
  if (!client.connected()) {
    // Silently return if not connected. The handleMqttClient function will show reconnection messages.
    return false;
  }

  // Use the dedicated builder to create the payload
  char payload[256];
  size_t payload_size = JsonBuilder::buildPayload(data, payload, sizeof(payload));

  if (payload_size == 0) return false; // JSON serialization failed

  if (!client.publish(topic, payload, payload_size)) {
#ifdef ARDUINO
      Serial.println("[MQTT] Publish failed. Message may be too large for MQTT buffer.");
#endif
      return false;
  }
  return true;
}

#ifdef ARDUINO
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
#endif // ARDUINO