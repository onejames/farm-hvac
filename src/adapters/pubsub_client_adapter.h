#ifndef PUBSUB_CLIENT_ADAPTER_H
#define PUBSUB_CLIENT_ADAPTER_H

#include "interfaces/i_mqtt_client.h"
#include <PubSubClient.h>

class PubSubClientAdapter : public IMqttClient {
public:
    explicit PubSubClientAdapter(PubSubClient& client);
    bool connected() override;
    bool publish(const char* topic, const uint8_t* payload, size_t length) override;

private:
    PubSubClient& _client;
};

#endif // PUBSUB_CLIENT_ADAPTER_H