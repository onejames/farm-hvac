#ifndef I_PUBSUB_CLIENT_H
#define I_PUBSUB_CLIENT_H

#include <cstdint>

class IPubSubClient {
public:
    virtual ~IPubSubClient() = default;

    virtual bool connect(const char* id) = 0;
    virtual bool connected() = 0;
    virtual void loop() = 0;
    virtual bool publish(const char* topic, const uint8_t* payload, unsigned int plength) = 0;
    virtual int state() = 0;
};

#endif // I_PUBSUB_CLIENT_H