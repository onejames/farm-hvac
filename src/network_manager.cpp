#include "network_manager.h"
#include "secrets.h"
#include "hvac_data.h"

// Required libraries for this module
#include <WiFiClientSecure.h>
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>

// --- Extern objects ---
// We still need extern for the network client objects defined in the main sketch.

// Network clients
extern WiFiClientSecure net;
extern PubSubClient client;
extern AsyncWebServer server;

// --- Private helper functions (static) ---

static String buildHtmlPage(const HVACData& data) {
  // Builds the HTML string to send to the client (FR-3.3)
  String html = "<!DOCTYPE html><html><head><title>HVAC Monitor</title>";
  html += "<meta http-equiv='refresh' content='5'></head><body>";
  html += "<h1>HVAC Real-Time Status</h1>";
  html += "<h2>Temperatures</h2>";
  html += "<p>Return Air: " + String(data.returnTempC, 1) + " &deg;C</p>";
  html += "<p>Supply Air: " + String(data.supplyTempC, 1) + " &deg;C</p>";
  html += "<p><b>Delta T: " + String(data.deltaT, 1) + " &deg;C</b></p>";
  html += "<h2>Component Status</h2>";
  html += "<p>Fan: " + data.fanStatus + " (" + String(data.fanAmps, 2) + " A)</p>";
  html += "<p>Compressor: " + data.compressorStatus + " (" + String(data.compressorAmps, 2) + " A)</p>";
  html += "<p>Geothermal Pumps: " + data.geoPumpsStatus + " (" + String(data.geoPumpsAmps, 2) + " A)</p>";
  html += "<h2>System</h2>";
  html += "<p>Airflow: " + data.airflowStatus + "</p>";
  html += "<p><b>Alerts: " + data.alertStatus + "</b></p>";
  html += "</body></html>";
  return html;
}

// --- Public function implementations ---

void connectAWS() {
  // Configure WiFiClientSecure to use the AWS certificates
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to Wi-Fi
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

  // Configure the MQTT client
  client.setServer(AWS_IOT_ENDPOINT, 8883);

  // Connect to AWS IoT
  Serial.print("[MQTT] Connecting to AWS IoT...");
  while (!client.connect(THINGNAME)) {
    Serial.print(".");
    delay(1000);
  }

  if (client.connected()) {
    Serial.println(" Connected!");
  } else {
    Serial.println(" Connection Failed!");
  }
}

void publishMessage(const HVACData& data) {
  if (!client.connected()) {
    Serial.println("[MQTT] Client not connected, skipping publish.");
    return;
  }

  String payload = "{";
  payload += "\"returnTempC\":" + String(data.returnTempC, 2) + ",";
  payload += "\"supplyTempC\":" + String(data.supplyTempC, 2) + ",";
  payload += "\"deltaT\":" + String(data.deltaT, 2) + ",";
  payload += "\"fanAmps\":" + String(data.fanAmps, 2) + ",";
  payload += "\"compressorAmps\":" + String(data.compressorAmps, 2) + ",";
  payload += "\"geoPumpsAmps\":" + String(data.geoPumpsAmps, 2) + ",";
  payload += "\"fanStatus\":\"" + data.fanStatus + "\",";
  payload += "\"compressorStatus\":\"" + data.compressorStatus + "\",";
  payload += "\"geoPumpsStatus\":\"" + data.geoPumpsStatus + "\",";
  payload += "\"airflowStatus\":\"" + data.airflowStatus + "\",";
  payload += "\"alertStatus\":\"" + data.alertStatus + "\"";
  payload += "}";

  client.publish(AWS_IOT_TOPIC, payload.c_str());
}

void setupWebServer(HVACData& data) {
  // The lambda captures a reference to 'data' so it always serves fresh values
  server.on("/", HTTP_GET, [&data](AsyncWebServerRequest *request){
    String html = buildHtmlPage(data);
    request->send(200, "text/html", html);
  });
  server.begin();
  Serial.println("[SETUP] Web server started.");
}

void handleMqttClient() {
  if (WiFi.status() == WL_CONNECTED && !client.connected()) {
    Serial.println("[MQTT] Disconnected, attempting to reconnect...");
    connectAWS(); // This will block until reconnected
  }
  client.loop();
}