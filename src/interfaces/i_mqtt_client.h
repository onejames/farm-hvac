#ifndef I_MQTT_CLIENT_H
#define I_MQTT_CLIENT_H

#include <cstddef> // for size_t

class IMqttClient {
public:
    virtual ~IMqttClient() = default;
    virtual bool connected() = 0;
    virtual bool publish(const char* topic, const char* payload, size_t length) = 0;
};
#endif // I_MQTT_CLIENT_H