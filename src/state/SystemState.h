#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include "hvac_data.h"
#include "config.h"
#include <array>

class SystemState {
public:
    SystemState();

    // Methods to access data
    [[nodiscard]] HVACData& getLatestData();
    [[nodiscard]] const std::array<HVACData, DATA_BUFFER_SIZE>& getDataBuffer() const;
    [[nodiscard]] const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& getAggregatedDataBuffer() const;
    [[nodiscard]] size_t getBufferIndex() const;
    [[nodiscard]] size_t getAggregatedBufferIndex() const;

    // Methods to modify state
    void recordLatestData();
    void addAggregatedData(const AggregatedHVACData& data);

private:
    HVACData _hvacData;
    std::array<HVACData, DATA_BUFFER_SIZE> _dataBuffer;
    size_t _dataBufferIndex;
    std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE> _aggregatedDataBuffer;
    size_t _aggregatedDataBufferIndex;
};

#endif // SYSTEM_STATE_H