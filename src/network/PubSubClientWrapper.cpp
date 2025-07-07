#include "PubSubClientWrapper.h"

#ifdef ARDUINO
// This is the full implementation for the Arduino hardware build
PubSubClientWrapper::PubSubClientWrapper(PubSubClient& client) : _client(client) {}

bool PubSubClientWrapper::connect(const char* id) {
    return _client.connect(id);
}

bool PubSubClientWrapper::connected() {
    return _client.connected();
}

void PubSubClientWrapper::loop() {
    _client.loop();
}

bool PubSubClientWrapper::publish(const char* topic, const uint8_t* payload, unsigned int plength) {
    return _client.publish(topic, payload, plength);
}

int PubSubClientWrapper::state() {
    return _client.state();
}
#else
// These are the "hollow" implementations for the native build environment.
PubSubClientWrapper::PubSubClientWrapper() {}
bool PubSubClientWrapper::connect(const char* /*id*/) { return true; }
bool PubSubClientWrapper::connected() { return false; }
void PubSubClientWrapper::loop() {}
bool PubSubClientWrapper::publish(const char* /*topic*/, const uint8_t* /*payload*/, unsigned int /*plength*/) { return true; }
int PubSubClientWrapper::state() { return -1; } // Return a "disconnected" state
#endif