#include <unity.h>
#include "network/MqttManager.h"
#include "state/SystemState.h"
#include "logging/log_manager.h"
#include "mocks/MockFileSystem.h"
#include "mocks/Arduino.h"
#include "secrets.h"
#include "mocks/MockMqttClient.h"

void setUp(void) {
    // Reset mock time before each test
    set_mock_millis(0);
}

void tearDown(void) {}

void test_handleClient_attempts_reconnect_when_disconnected() {
    // Arrange
    SystemState systemState;
    MockFileSystem mockFS;
    LogManager logManager(mockFS);
    auto mockMqttClient = std::make_unique<MockMqttClient>();
    MockMqttClient* mockClientPtr = mockMqttClient.get(); // Get raw pointer for inspection
    mockClientPtr->_connected = false;
    // Advance time to ensure the reconnect interval is exceeded
    set_mock_millis(6000);
    MqttManager mqttManager(systemState, logManager, std::move(mockMqttClient));

    // Act
    mqttManager.handleClient();

    // Assert
    TEST_ASSERT_TRUE(mockClientPtr->connect_called);
}

void test_handleClient_calls_loop_when_connected() {
    // Arrange
    SystemState systemState;
    MockFileSystem mockFS;
    LogManager logManager(mockFS);
    auto mockMqttClient = std::make_unique<MockMqttClient>();
    MockMqttClient* mockClientPtr = mockMqttClient.get(); // Get raw pointer for inspection
    mockClientPtr->_connected = true;
    MqttManager mqttManager(systemState, logManager, std::move(mockMqttClient));

    // Act
    mqttManager.handleClient();

    // Assert
    TEST_ASSERT_TRUE(mockClientPtr->loop_called);
    TEST_ASSERT_FALSE(mockClientPtr->connect_called);
}

void test_publishAggregatedData_sends_correct_payload() {
    // Arrange
    SystemState systemState;
    MockFileSystem mockFS;
    LogManager logManager(mockFS);
    auto mockMqttClient = std::make_unique<MockMqttClient>();
    MockMqttClient* mockClientPtr = mockMqttClient.get(); // Get a raw pointer for inspection before moving
    mockClientPtr->_connected = true; // Corrected: client must be connected to publish
    MqttManager mqttManager(systemState, logManager, std::move(mockMqttClient));
    AggregatedHVACData aggData;
    aggData.avgReturnTempC = 22.5f;
    aggData.timestamp = 12345;
    systemState.addAggregatedData(aggData);

    // Act
    mqttManager.publishAggregatedData();

    // Assert
    TEST_ASSERT_EQUAL_STRING(AWS_IOT_TOPIC, mockClientPtr->last_topic.c_str());
    TEST_ASSERT_TRUE(mockClientPtr->last_payload.find("\"avgReturnTempC\":22.5") != std::string::npos);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_handleClient_attempts_reconnect_when_disconnected);
    RUN_TEST(test_handleClient_calls_loop_when_connected);
    RUN_TEST(test_publishAggregatedData_sends_correct_payload);
    return UNITY_END();
}