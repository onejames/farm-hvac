#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>

// Forward declare dependencies to reduce header includes
class SystemState;
class ConfigManager;
class LogManager;

class NetworkManager {
public:
    explicit NetworkManager(SystemState& systemState,
                           ConfigManager& configManager,
                           LogManager& logManager);

    void setup();
    void handleClient();
    void publishAggregatedData();

private:
    void setupWiFi();
    void setupWebInterface();
    void setupMqtt();
    void setupApiRoutes();
    void setupSettingsRoutes();
    void setupSystemRoutes();
    void setupStaticFileServer();

    SystemState& _systemState;
    ConfigManager& _configManager;
    LogManager& _logManager;

    WiFiClientSecure _net;
    PubSubClient _client;
    AsyncWebServer _server;
    long _lastMqttReconnectAttempt;
};

#endif // NETWORK_MANAGER_H