#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hvac_data.h"
#include "interfaces/i_mqtt_client.h"

#ifdef ARDUINO
void setupWiFi();
void setupWebInterface();
void handleMqttClient();
void publishMessage();
#endif

/**
 * @brief Publishes an MQTT message. This function is platform-agnostic and testable.
 */
bool publishMqttMessage(IMqttClient& client, const char* topic, const HVACData& data, const char* version, const char* buildDate);

#endif // NETWORK_MANAGER_H