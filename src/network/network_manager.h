#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "hvac_data.h"
#include "interfaces/i_mqtt_client.h"

class NetworkManager {
public:
    explicit NetworkManager(HVACData& data);
    void setup();
    void handleClient();
    void publish();

private:
    void setupWiFi();
    void setupWebInterface();
    bool publishMqttMessage(IMqttClient& client, const char* topic, const char* version, const char* buildDate);

    HVACData& _hvacData;
    WiFiClientSecure _net;
    PubSubClient _client;
    AsyncWebServer _server;
    unsigned long _lastMqttReconnectAttempt;
};

#endif // NETWORK_MANAGER_H