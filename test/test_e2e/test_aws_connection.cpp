#include <unity.h>
#include <Arduino.h>

// We need to include all the necessary headers because we are not running main.cpp
#include "secrets.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "network/network_manager.h" // For publishMqttMessage
#include "adapters/pubsub_client_adapter.h"
#include "hvac_data.h"

// Global objects for the test
WiFiClientSecure net;
PubSubClient client(net);

void connect_wifi() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    int retries = 20;
    while (WiFi.status() != WL_CONNECTED && retries > 0) {
        delay(500);
        Serial.print(".");
        retries--;
    }
    Serial.println();
    TEST_ASSERT_EQUAL_MESSAGE(WL_CONNECTED, WiFi.status(), "Failed to connect to WiFi. Check credentials in secrets.h");
}

void connect_aws_mqtt() {
    // Configure the secure client
    net.setCACert(AWS_CERT_CA);
    net.setCertificate(AWS_CERT_CRT);
    net.setPrivateKey(AWS_CERT_PRIVATE);

    // Configure the MQTT client
    client.setServer(AWS_IOT_ENDPOINT, 8883);

    // Attempt to connect
    Serial.print("Connecting to AWS IoT...");
    int retries = 5;
    while (!client.connect(THINGNAME) && retries > 0) {
        delay(1000);
        Serial.print(".");
        retries--;
    }
    Serial.println();
    TEST_ASSERT_TRUE_MESSAGE(client.connected(), "Failed to connect to AWS IoT MQTT. Check certificates and endpoint in secrets.h");
}

void test_publish_to_aws_succeeds(void) {
    // 1. Arrange
    connect_wifi();
    connect_aws_mqtt();

    // Create the adapter and sample data
    PubSubClientAdapter mqttAdapter(client);
    HVACData testData;
    testData.returnTempC = 99.9;
    testData.fanStatus = "TESTING";
    testData.alertStatus = "E2E_TEST_OK";

    // 2. Act
    Serial.println("Publishing test message...");
    bool result = publishMqttMessage(mqttAdapter, AWS_IOT_TOPIC, testData, "e2e-test", "2025-01-01");

    // 3. Assert
    TEST_ASSERT_TRUE_MESSAGE(result, "NetworkManager::publish returned false.");
    Serial.println("Publish command sent successfully.");
    Serial.println("\n---> Please check the AWS IoT MQTT Test Client to verify message reception. <---");

    // Give a moment for the message to be sent and disconnect
    delay(1000);
    client.disconnect();
    WiFi.disconnect();
}

// For on-device tests, we need to define setup and loop for the test runner.
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_publish_to_aws_succeeds);
    UNITY_END();
}

void loop() {
    // The test runs in setup(), so loop is empty.
}