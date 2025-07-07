#ifndef MOCK_MQTT_CLIENT_H
#define MOCK_MQTT_CLIENT_H

#include "network/IPubSubClient.h"
#include <string>
#include <vector>

class MockMqttClient : public IPubSubClient {
public:
    MockMqttClient() = default;

    // Test control variables
    bool _connected = false;
    bool connect_retval = true;
    bool publish_retval = true;
    int state_retval = 0;

    // Test inspection variables
    std::string last_topic;
    std::string last_payload;
    bool loop_called = false;
    bool connect_called = false;

    // Mocked methods
    bool connected() override { return _connected; };
    int state() override { return state_retval; };
    void loop() override { loop_called = true; };

    bool connect(const char* id) override {
        connect_called = true;
        _connected = connect_retval;
        return connect_retval;
    };

    bool publish(const char* topic, const uint8_t* payload, unsigned int plength) override {
        last_topic = topic;
        last_payload = std::string(reinterpret_cast<const char*>(payload), plength);
        return publish_retval;
    };
};

#endif // MOCK_MQTT_CLIENT_H