#include <unity.h>
#include "logic/data_aggregator.h"
#include "hvac_data.h"
#include "config.h"
#include <array>

void setUp(void) {}
void tearDown(void) {}

void test_aggregate_calculates_averages_correctly() {
    // 1. Arrange
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData()); // Start with a clean buffer

    HVACData d1;
    d1.returnTempC = 20.0f;
    d1.fanAmps = 1.0;
    d1.isInitialized = true;

    HVACData d2;
    d2.returnTempC = 30.0f;
    d2.fanAmps = 2.0;
    d2.isInitialized = true;

    buffer[0] = d1;
    buffer[1] = d2;

    // 2. Act
    // The last known data isn't used for averaging, so a default is fine here.
    AggregatedHVACData result = DataAggregator::aggregate(buffer, HVACData());

    // 3. Assert
    TEST_ASSERT_EQUAL_FLOAT(25.0f, result.avgReturnTempC); // (20 + 30) / 2
    TEST_ASSERT_EQUAL_FLOAT(1.5, result.avgFanAmps);       // (1.0 + 2.0) / 2
}

void test_aggregate_handles_partially_filled_buffer() {
    // This test ensures that default-initialized entries are skipped
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());

    HVACData d1;
    d1.returnTempC = 10.0f;
    d1.isInitialized = true;
    buffer[5] = d1; // Put one valid entry in the middle

    AggregatedHVACData result = DataAggregator::aggregate(buffer, HVACData());

    TEST_ASSERT_EQUAL_FLOAT(10.0f, result.avgReturnTempC);
}

void test_aggregate_handles_empty_buffer() {
    // This test ensures no division-by-zero errors
    std::array<HVACData, DATA_BUFFER_SIZE> buffer;
    buffer.fill(HVACData());

    AggregatedHVACData result = DataAggregator::aggregate(buffer, HVACData());

    TEST_ASSERT_EQUAL_FLOAT(0.0f, result.avgReturnTempC);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, result.avgFanAmps);
}

void test_aggregate_captures_last_known_status() {
    // 1. Arrange
    std::array<HVACData, DATA_BUFFER_SIZE> buffer; // Buffer content doesn't matter for this test
    buffer.fill(HVACData());

    HVACData lastKnown;
    lastKnown.fanStatus = ComponentStatus::ON;
    lastKnown.compressorStatus = ComponentStatus::OFF;
    lastKnown.geoPumpsStatus = ComponentStatus::UNKNOWN;

    // 2. Act
    AggregatedHVACData result = DataAggregator::aggregate(buffer, lastKnown);

    // 3. Assert
    TEST_ASSERT_EQUAL(ComponentStatus::ON, result.lastFanStatus);
    TEST_ASSERT_EQUAL(ComponentStatus::OFF, result.lastCompressorStatus);
    TEST_ASSERT_EQUAL(ComponentStatus::UNKNOWN, result.lastGeoPumpsStatus);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_aggregate_calculates_averages_correctly);
    RUN_TEST(test_aggregate_handles_partially_filled_buffer);
    RUN_TEST(test_aggregate_handles_empty_buffer);
    RUN_TEST(test_aggregate_captures_last_known_status);
    return UNITY_END();
}