#include <unity.h>
#include "DataManager.h"
#include "hardware/IHardwareManager.h"
#include "config.h" // For device addresses
#include "interfaces/i_temperature_sensor.h"
#include "interfaces/i_current_sensor.h"

// --- Mocks for Dependencies ---

class MockTemperatureSensor : public ITemperatureSensor {
public:
    float returnTemp = 0.0f;
    float supplyTemp = 0.0f;
    bool requestTemperaturesCalled = false;

    void requestTemperatures() override {
        requestTemperaturesCalled = true;
    }

    float getTempC(const DeviceAddress& deviceAddress) override {
        // A simple way to distinguish between the two sensors for the mock
        // In a real scenario, we might compare the full address.
        if (deviceAddress[7] == returnAirSensorAddress[7]) {
            return returnTemp;
        }
        if (deviceAddress[7] == supplyAirSensorAddress[7]) {
            return supplyTemp;
        }
        return -127.0f; // Default error value
    }
};

class MockCurrentSensor : public ICurrentSensor {
public:
    double amps = 0.0;

    double calcIrms(unsigned int samples) override {
        return amps;
    }
};

// --- Test Suite ---

class MockHardwareManager : public IHardwareManager {
public:
    MockTemperatureSensor mockTempSensor;
    MockCurrentSensor mockFanSensor;
    MockCurrentSensor mockCompressorSensor;
    MockCurrentSensor mockPumpsSensor;

    void setup() override {}

    ITemperatureSensor& getTempAdapter() override { return mockTempSensor; }
    ICurrentSensor& getFanAdapter() override { return mockFanSensor; }
    ICurrentSensor& getCompressorAdapter() override { return mockCompressorSensor; }
    ICurrentSensor& getPumpsAdapter() override { return mockPumpsSensor; }
};

void setUp(void) {}

void tearDown(void) {}

void test_readAndProcessData_calculates_deltaT_correctly() {
    // Arrange
    MockHardwareManager mockHardwareManager;
    DataManager dataManager(mockHardwareManager, returnAirSensorAddress, supplyAirSensorAddress);
    HVACData data;
    mockHardwareManager.mockTempSensor.returnTemp = 25.0f;
    mockHardwareManager.mockTempSensor.supplyTemp = 20.0f;

    // Act
    dataManager.readAndProcessData(data, 1, 0.5f);

    // Assert
    TEST_ASSERT_TRUE(mockHardwareManager.mockTempSensor.requestTemperaturesCalled);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, data.returnTempC);
    TEST_ASSERT_EQUAL_FLOAT(20.0f, data.supplyTempC);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, data.deltaT); // 25.0 - 20.0
}

void test_readAndProcessData_sets_component_status_correctly() {
    // Arrange
    MockHardwareManager mockHardwareManager;
    DataManager dataManager(mockHardwareManager, returnAirSensorAddress, supplyAirSensorAddress);
    HVACData data;
    const float threshold = 0.5f;

    // Set currents to be ON or OFF relative to the threshold
    mockHardwareManager.mockFanSensor.amps = 1.0;
    mockHardwareManager.mockCompressorSensor.amps = 2.0;
    mockHardwareManager.mockPumpsSensor.amps = 0.1; // OFF

    // Act
    dataManager.readAndProcessData(data, 1, threshold);

    // Assert
    TEST_ASSERT_EQUAL(ComponentStatus::ON, data.fanStatus);
    TEST_ASSERT_EQUAL(ComponentStatus::ON, data.compressorStatus);
    TEST_ASSERT_EQUAL(ComponentStatus::OFF, data.geoPumpsStatus);
}

void test_readAndProcessData_handles_disconnected_sensor() {
    // Arrange
    MockHardwareManager mockHardwareManager;
    DataManager dataManager(mockHardwareManager, returnAirSensorAddress, supplyAirSensorAddress);
    HVACData data;
    mockHardwareManager.mockTempSensor.returnTemp = -127.0f; // Disconnected value
    mockHardwareManager.mockTempSensor.supplyTemp = 20.0f;

    // Act
    dataManager.readAndProcessData(data, 1, 0.5f);

    // Assert
    TEST_ASSERT_EQUAL_FLOAT(-127.0f, data.returnTempC);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, data.deltaT); // Delta T should be 0 if a sensor is disconnected
}

void test_readAndProcessData_sets_isInitialized_flag() {
    // Arrange
    MockHardwareManager mockHardwareManager;
    DataManager dataManager(mockHardwareManager, returnAirSensorAddress, supplyAirSensorAddress);
    HVACData data;
    TEST_ASSERT_FALSE(data.isInitialized); // Verify it's false initially

    // Act
    dataManager.readAndProcessData(data, 1, 0.5f);

    // Assert
    TEST_ASSERT_TRUE(data.isInitialized);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_readAndProcessData_calculates_deltaT_correctly);
    RUN_TEST(test_readAndProcessData_sets_component_status_correctly);
    RUN_TEST(test_readAndProcessData_handles_disconnected_sensor);
    RUN_TEST(test_readAndProcessData_sets_isInitialized_flag);
    return UNITY_END();
}