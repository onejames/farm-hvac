#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "hvac_data.h"

/**
 * @brief Sets up Wi-Fi connection and configures AWS IoT certificates.
 * This is a blocking function intended to be called once from setup().
 */
void setupNetwork();

/**
 * @brief Handles the MQTT client connection state. Should be called in the main loop.
 * Manages non-blocking reconnection logic.
 */
void handleMqttClient();

/**
 * @brief Publishes the HVAC data to AWS IoT as a JSON payload.
 * @param data The HVACData struct containing the latest sensor readings.
 */
void publishMessage(const HVACData& data);

/**
 * @brief Sets up the asynchronous web server to display HVAC data.
 * @param data A reference to the HVACData struct to ensure the web page
 *             always displays the most current data.
 */
void setupWebServer(HVACData& data);

#endif // NETWORK_MANAGER_H