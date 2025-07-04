#include "network_manager.h"
#include "secrets.h"
#include "hvac_data.h"

// Required libraries for this module
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// --- Extern objects ---
// We still need extern for the network client objects defined in the main sketch.

// Network clients
extern WiFiClientSecure net;
extern PubSubClient client;
extern AsyncWebServer server;

// --- State for non-blocking reconnect ---
static unsigned long lastMqttReconnectAttempt = 0;
const long mqttReconnectInterval = 5000; // Attempt reconnect every 5 seconds


// --- Public function implementations ---

void setupNetwork() {
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
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Configure the MQTT client
  client.setServer(AWS_IOT_ENDPOINT, 8883);
}

void publishMessage(const HVACData& data) {
  if (!client.connected()) {
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

  // Publish the message. Note: PubSubClient's default buffer is 256 bytes.
  // If this fails, the payload might be too large.
  if (!client.publish(AWS_IOT_TOPIC, payload, n)) {
      Serial.println("[MQTT] Publish failed. Message may be too large for MQTT buffer.");
  }
}

void setupWebServer(HVACData& data) {
  // The lambda captures a reference to 'data' so it always serves fresh values
  server.on("/", HTTP_GET, [&data](AsyncWebServerRequest *request){
    // Use a response stream to build the HTML page without large String allocations.
    // This is much more memory-efficient and helps prevent heap fragmentation.
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>HVAC Monitor</title>");
    response->print("<meta http-equiv='refresh' content='5'></head><body>");
    response->print("<h1>HVAC Real-Time Status</h1>");
    response->print("<h2>Temperatures</h2>");
    response->printf("<p>Return Air: %.1f &deg;C</p>", data.returnTempC);
    response->printf("<p>Supply Air: %.1f &deg;C</p>", data.supplyTempC);
    response->printf("<p><b>Delta T: %.1f &deg;C</b></p>", data.deltaT);
    response->print("<h2>Component Status</h2>");
    response->printf("<p>Fan: %s (%.2f A)</p>", data.fanStatus.c_str(), data.fanAmps);
    response->printf("<p>Compressor: %s (%.2f A)</p>", data.compressorStatus.c_str(), data.compressorAmps);
    response->printf("<p>Geothermal Pumps: %s (%.2f A)</p>", data.geoPumpsStatus.c_str(), data.geoPumpsAmps);
    response->print("<h2>System</h2>");
    response->printf("<p>Airflow: %s</p>", data.airflowStatus.c_str());
    response->printf("<p><b>Alerts: %s</b></p>", data.alertStatus.c_str());
    response->print("</body></html>");

    request->send(response);
  });
  server.begin();
  Serial.println("[SETUP] Web server started.");
}

void handleMqttClient() {
  if (!client.connected()) {
    long now = millis();
    // Check if it's time to try connecting
    if (now - lastMqttReconnectAttempt > mqttReconnectInterval) {
      lastMqttReconnectAttempt = now;
      Serial.print("[MQTT] Attempting to connect to AWS IoT...");
      if (client.connect(THINGNAME)) {
        Serial.println(" Connected!");
        // Optional: subscribe to topics here if needed
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