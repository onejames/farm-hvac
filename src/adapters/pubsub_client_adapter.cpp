#include "pubsub_client_adapter.h"

PubSubClientAdapter::PubSubClientAdapter(PubSubClient& client) : _client(client) {}

bool PubSubClientAdapter::connected() {
    return _client.connected();
}

bool PubSubClientAdapter::publish(const char* topic, const char* payload, size_t length) {
    return _client.publish(topic, reinterpret_cast<const uint8_t*>(payload), length);
}