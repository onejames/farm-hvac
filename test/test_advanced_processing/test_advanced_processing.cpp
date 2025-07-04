#include <unity.h>
#include "data_processing.h"
#include "hvac_data.h"

#include <map>
#include <vector>

// --- Mock Implementations ---
// A fake temperature sensor that implements our interface
class MockTemperatureSensor : public ITemperatureSensor {
public:
    // Use a map to store different temperatures for different addresses
    std::map<std::vector<uint8_t>, float> temps;

    void setTemp(const DeviceAddress& address, float temp) {
        std::vector<uint8_t> addrVec(address, address + 8);
        temps[addrVec] = temp;
    }

    void requestTemperatures() override {} // Do nothing
    float getTempC(const DeviceAddress& address) override {
        std::vector<uint8_t> addrVec(address, address + 8);
        if (temps.count(addrVec)) {
            return temps[addrVec];
        }
        // Return a default error value if the address hasn't been set in the mock
        return -127.0f;
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

    // Dummy addresses, which now matter for the more advanced mock
    DeviceAddress returnAddr = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    DeviceAddress supplyAddr = {0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01};

    // Configure the mock return values for specific addresses
    mockTemp.setTemp(returnAddr, 25.0f);
    mockTemp.setTemp(supplyAddr, 18.0f);
    mockFan.irmsToReturn = 1.5;
    mockCompressor.irmsToReturn = 5.8;
    mockPumps.irmsToReturn = 0.2;

    // Create the DataManager, injecting our MOCK objects
    DataManager dm(mockTemp, mockFan, mockCompressor, mockPumps, returnAddr, supplyAddr);

    // 2. Act
    dm.readAllSensors(data, 1480); // Pass a dummy sample count

    // 3. Assert
    TEST_ASSERT_EQUAL_FLOAT(25.0, data.returnTempC);
    TEST_ASSERT_EQUAL_FLOAT(18.0, data.supplyTempC);
    TEST_ASSERT_EQUAL_FLOAT(1.5, data.fanAmps);
    TEST_ASSERT_EQUAL_FLOAT(5.8, data.compressorAmps);
    TEST_ASSERT_EQUAL_FLOAT(0.2, data.geoPumpsAmps);
}

void test_readAllSensors_handles_temp_sensor_error(void) {
    // 1. Arrange
    HVACData data;
    MockTemperatureSensor mockTemp;
    MockCurrentSensor mockFan, mockCompressor, mockPumps;

    // Configure the mock to return an error code for only one sensor
    DeviceAddress returnAddr = {0x01};
    DeviceAddress supplyAddr = {0x02};
    mockTemp.setTemp(returnAddr, 25.0f);
    // We don't set a temp for supplyAddr, so it will return the default error code.

    DataManager dm(mockTemp, mockFan, mockCompressor, mockPumps, returnAddr, supplyAddr);

    // 2. Act
    dm.readAllSensors(data, 1480);

    // 3. Assert
    TEST_ASSERT_EQUAL_FLOAT(25.0, data.returnTempC);
    TEST_ASSERT_EQUAL_FLOAT(-127.0, data.supplyTempC);
}

// This main function is the entry point for this specific test suite.
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readAllSensors_populates_data_correctly_from_mocks);
    RUN_TEST(test_readAllSensors_handles_temp_sensor_error);
    return UNITY_END();
}