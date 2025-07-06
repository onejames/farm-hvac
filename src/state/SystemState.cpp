#include "SystemState.h"

SystemState::SystemState()
    : _dataBufferIndex(0),
      _aggregatedDataBufferIndex(0)
{}

HVACData& SystemState::getLatestData() {
    return _hvacData;
}

const std::array<HVACData, DATA_BUFFER_SIZE>& SystemState::getDataBuffer() const {
    return _dataBuffer;
}

const std::array<AggregatedHVACData, AGGREGATED_DATA_BUFFER_SIZE>& SystemState::getAggregatedDataBuffer() const {
    return _aggregatedDataBuffer;
}

size_t SystemState::getBufferIndex() const {
    return _dataBufferIndex;
}

size_t SystemState::getAggregatedBufferIndex() const {
    return _aggregatedDataBufferIndex;
}

void SystemState::recordLatestData() {
    _dataBuffer[_dataBufferIndex] = _hvacData;
    _dataBufferIndex = (_dataBufferIndex + 1) % DATA_BUFFER_SIZE;
}

void SystemState::addAggregatedData(const AggregatedHVACData& data) {
    _aggregatedDataBuffer[_aggregatedDataBufferIndex] = data;
    _aggregatedDataBufferIndex = (_aggregatedDataBufferIndex + 1) % AGGREGATED_DATA_BUFFER_SIZE;
}