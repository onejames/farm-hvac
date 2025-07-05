#include "pubsub_client_adapter.h"

PubSubClientAdapter::PubSubClientAdapter(PubSubClient& client) : _client(client) {}

bool PubSubClientAdapter::connected() {
    return _client.connected();
}

bool PubSubClientAdapter::publish(const char* topic, const uint8_t* payload, size_t length) {
    // The PubSubClient::publish method takes an unsigned int for length.
    // We perform a static_cast to ensure type compatibility.
    return _client.publish(topic, payload, static_cast<unsigned int>(length));
}