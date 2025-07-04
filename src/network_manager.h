#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hvac_data.h"

// Function declarations for networking and web server
void connectAWS();
void publishMessage(const HVACData& data);
void setupWebServer(HVACData& data);
void handleMqttClient();

#endif // NETWORK_MANAGER_H