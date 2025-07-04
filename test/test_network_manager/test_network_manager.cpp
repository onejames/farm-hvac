#include <unity.h>
#include "network_manager.h"
#include "hvac_data.h"
#include <ArduinoJson.h>
#include "interfaces/i_mqtt_client.h" // Include the full definition of the base class

// In a test environment, we don't link against secrets.h, so we must define
// any constants we need for the test.
const char AWS_IOT_TOPIC[] = "hvac/data";

// --- Mock MQTT Client ---
class MockMqttClient : public IMqttClient {
public:
    bool publish_return_value = true;
    bool connected_return_value = true;
    bool publish_was_called = false;
    std::string last_topic;
    std::string last_payload;

    bool connected() override {
        return connected_return_value;
    }

    bool publish(const char* topic, const char* payload, size_t length) override {
        publish_was_called = true;
        last_topic = topic;
        last_payload = std::string(payload, length);
        return publish_return_value;
    }
};

void setUp(void) {}
void tearDown(void) {}

void test_publish_sends_correct_json_payload(void) {
    // 1. Arrange
    MockMqttClient mockClient;
    HVACData data;
    data.returnTempC = 24.5;
    data.supplyTempC = 19.0;
    data.deltaT = 5.5;
    data.fanAmps = 1.23;
    data.compressorAmps = 4.56;
    data.geoPumpsAmps = 0.78;
    data.fanStatus = "ON";
    data.compressorStatus = "ON";
    data.geoPumpsStatus = "OFF";
    data.airflowStatus = "OK";
    data.alertStatus = "NONE";

    const char* test_version = "v-test-1";
    const char* test_build_date = "2025-10-26";

    // 2. Act
    bool result = NetworkManager::publish(mockClient, AWS_IOT_TOPIC, data, test_version, test_build_date);

    // 3. Assert
    TEST_ASSERT_TRUE(mockClient.publish_was_called);
    TEST_ASSERT_EQUAL_STRING(AWS_IOT_TOPIC, mockClient.last_topic.c_str());

    // Verify the JSON payload content
    JsonDocument doc;
    TEST_ASSERT_TRUE(result);
    DeserializationError error = deserializeJson(doc, mockClient.last_payload);
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_EQUAL_FLOAT(24.5, doc["returnTempC"]);
    TEST_ASSERT_EQUAL_FLOAT(19.0, doc["supplyTempC"]);
    TEST_ASSERT_EQUAL_FLOAT(5.5, doc["deltaT"]);
    TEST_ASSERT_EQUAL_FLOAT(1.23, doc["fanAmps"]);
    TEST_ASSERT_EQUAL_FLOAT(4.56, doc["compressorAmps"]);
    TEST_ASSERT_EQUAL_FLOAT(0.78, doc["geoPumpsAmps"]);
    TEST_ASSERT_EQUAL_STRING("ON", doc["fanStatus"]);
    TEST_ASSERT_EQUAL_STRING("ON", doc["compressorStatus"]);
    TEST_ASSERT_EQUAL_STRING("OFF", doc["geoPumpsStatus"]);
    TEST_ASSERT_EQUAL_STRING("NONE", doc["alertStatus"]);
    TEST_ASSERT_EQUAL_STRING("OK", doc["airflowStatus"]);
    TEST_ASSERT_EQUAL_STRING(test_version, doc["version"]);
    TEST_ASSERT_EQUAL_STRING(test_build_date, doc["buildDate"]);
}

void test_publish_does_not_send_when_disconnected(void) {
    // 1. Arrange
    MockMqttClient mockClient;
    mockClient.connected_return_value = false; // Set the mock to be disconnected
    HVACData data; // Content doesn't matter

    // 2. Act
    bool result = NetworkManager::publish(mockClient, AWS_IOT_TOPIC, data, "v", "d");

    // 3. Assert
    TEST_ASSERT_FALSE(mockClient.publish_was_called);
    TEST_ASSERT_FALSE(result);
}

void test_publish_returns_false_when_client_publish_fails(void) {
    // 1. Arrange
    MockMqttClient mockClient;
    mockClient.publish_return_value = false; // Tell the mock to simulate a failure
    HVACData data; // Content doesn't matter

    // 2. Act
    bool result = NetworkManager::publish(mockClient, AWS_IOT_TOPIC, data, "v", "d");

    // 3. Assert
    TEST_ASSERT_TRUE(mockClient.publish_was_called); // It should still be called
    TEST_ASSERT_FALSE(result); // The overall function should return false
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_publish_sends_correct_json_payload);
    RUN_TEST(test_publish_does_not_send_when_disconnected);
    RUN_TEST(test_publish_returns_false_when_client_publish_fails);
    return UNITY_END();
}