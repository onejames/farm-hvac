#ifndef I_MQTT_CLIENT_H
#define I_MQTT_CLIENT_H

#include <stddef.h>
#include <stdint.h>

class IMqttClient {
public:
    virtual ~IMqttClient() = default;
    virtual bool connected() = 0;
    virtual bool publish(const char* topic, const uint8_t* payload, size_t length) = 0;
};

#endif // I_MQTT_CLIENT_H