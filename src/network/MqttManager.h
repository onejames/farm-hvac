#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <memory> // for std::unique_ptr

// Forward declare dependencies
class SystemState;
class LogManager;
class IPubSubClient;

class MqttManager {
public:
    explicit MqttManager(SystemState& systemState, LogManager& logManager, std::unique_ptr<IPubSubClient> client);
    ~MqttManager();

    void handleClient();
    void publishAggregatedData();

private:
    SystemState& _systemState;
    LogManager& _logManager;
    std::unique_ptr<IPubSubClient> _client;
    long _lastMqttReconnectAttempt;
};

#endif // MQTT_MANAGER_H