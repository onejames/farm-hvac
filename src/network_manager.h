#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hvac_data.h"

// Forward declarations to avoid including heavy headers here
class WiFiClientSecure;
class PubSubClient;
class IMqttClient;
class AsyncWebServer;

class NetworkManager {
public:
    NetworkManager(WiFiClientSecure& net, PubSubClient& client, AsyncWebServer& server);

    void setup(HVACData& data);
    void handleClient();
    static bool publish(IMqttClient& client, const char* topic, const HVACData& data, const char* version, const char* buildDate);

private:
    WiFiClientSecure& _net;
    PubSubClient& _client;
    AsyncWebServer& _server;

    unsigned long _lastMqttReconnectAttempt = 0;
};

#endif // NETWORK_MANAGER_H