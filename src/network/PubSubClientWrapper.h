#ifndef PUBSUB_CLIENT_WRAPPER_H
#define PUBSUB_CLIENT_WRAPPER_H

#include "IPubSubClient.h"
#ifdef ARDUINO
#include <PubSubClient.h>
#endif

class PubSubClientWrapper : public IPubSubClient {
public:
#ifdef ARDUINO
    explicit PubSubClientWrapper(PubSubClient& client);
#else
    PubSubClientWrapper(); // Default constructor for native builds
#endif

    bool connect(const char* id) override;
    bool connected() override;
    void loop() override;
    bool publish(const char* topic, const uint8_t* payload, unsigned int plength) override;
    int state() override;

private:
#ifdef ARDUINO
    PubSubClient& _client;
#endif
};

#endif // PUBSUB_CLIENT_WRAPPER_H