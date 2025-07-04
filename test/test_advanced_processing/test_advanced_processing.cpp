#include <unity.h>
#include "data_processing.h"
#include "hvac_data.h"

// --- Mock Implementations ---
// A fake temperature sensor that implements our interface
class MockTemperatureSensor : public ITemperatureSensor {
public:
    float tempToReturn = 0.0f;
    void requestTemperatures() override {} // Do nothing
    float getTempC(const DeviceAddress& address) override {
        return tempToReturn;
    }
};

// A fake current sensor that implements our interface
class MockCurrentSensor : public ICurrentSensor {
public:
    double irmsToReturn = 0.0;
    double calcIrms(unsigned int samples) override {
        return irmsToReturn;
    }
};


void setUp(void) {}
void tearDown(void) {}

void test_readAllSensors_populates_data_correctly_from_mocks(void) {
    // 1. Arrange
    HVACData data;
    MockTemperatureSensor mockTemp;
    MockCurrentSensor mockFan, mockCompressor, mockPumps;

    // Configure the mock return values
    mockTemp.tempToReturn = 22.5f;
    mockFan.irmsToReturn = 1.5;
    mockCompressor.irmsToReturn = 5.8;
    mockPumps.irmsToReturn = 0.2;

    // Dummy addresses, they don't matter for the mock
    DeviceAddress dummyAddr1, dummyAddr2;

    // Create the DataManager, injecting our MOCK objects
    DataManager dm(mockTemp, mockFan, mockCompressor, mockPumps, dummyAddr1, dummyAddr2);

    // 2. Act
    dm.readAllSensors(data, 1480); // Pass a dummy sample count

    // 3. Assert
    TEST_ASSERT_EQUAL_FLOAT(22.5, data.returnTempC);
    TEST_ASSERT_EQUAL_FLOAT(22.5, data.supplyTempC);
    TEST_ASSERT_EQUAL_FLOAT(1.5, data.fanAmps);
    TEST_ASSERT_EQUAL_FLOAT(5.8, data.compressorAmps);
    TEST_ASSERT_EQUAL_FLOAT(0.2, data.geoPumpsAmps);
}

void test_readAllSensors_handles_temp_sensor_error(void) {
    // 1. Arrange
    HVACData data;
    MockTemperatureSensor mockTemp;
    MockCurrentSensor mockFan, mockCompressor, mockPumps;

    // Configure the mock to return an error code
    mockTemp.tempToReturn = -127.0f;

    DeviceAddress dummyAddr1, dummyAddr2;
    DataManager dm(mockTemp, mockFan, mockCompressor, mockPumps, dummyAddr1, dummyAddr2);

    // 2. Act
    dm.readAllSensors(data, 1480);

    // 3. Assert
    TEST_ASSERT_EQUAL_FLOAT(-127.0, data.returnTempC);
}

// This main function is the entry point for this specific test suite.
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readAllSensors_populates_data_correctly_from_mocks);
    RUN_TEST(test_readAllSensors_handles_temp_sensor_error);
    return UNITY_END();
}