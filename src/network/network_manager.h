#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <array>
#include "config.h"
#include <ESPAsyncWebServer.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "hvac_data.h"
#include "interfaces/i_mqtt_client.h"

class ConfigManager; // Forward declaration
struct AggregatedHVACData; // Forward declaration

class NetworkManager {
public:
    explicit NetworkManager(HVACData& latestData,
                            const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const size_t& bufferIndex,
                            const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& aggregatedBuffer, const size_t& aggregatedBufferIndex,
                            ConfigManager& configManager);
    void setup();
    void handleClient();
    void publishAggregatedData();

private:
    void setupWiFi();
    void setupWebInterface();

    HVACData& _hvacData;
    const std::array<HVACData, DATA_BUFFER_SIZE>& _dataBuffer;
    const size_t& _bufferIndex;
    const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& _aggregatedDataBuffer;
    const size_t& _aggregatedBufferIndex;
    ConfigManager& _configManager;
    WiFiClientSecure _net;
    PubSubClient _client;
    AsyncWebServer _server;
    unsigned long _lastMqttReconnectAttempt;
};

#endif // NETWORK_MANAGER_H