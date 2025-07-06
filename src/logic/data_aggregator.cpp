#include "data_aggregator.h"

AggregatedHVACData DataAggregator::aggregate(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const HVACData& lastKnownData) {
    double sumReturnTemp = 0.0;
    double sumSupplyTemp = 0.0;
    double sumDeltaT = 0.0;
    double sumFanAmps = 0.0;
    double sumCompressorAmps = 0.0;
    double sumGeoPumpsAmps = 0.0;
    size_t validSamples = 0;

    for (const auto& data : dataBuffer) {
        // Skip uninitialized entries in the buffer
        if (!data.isInitialized) {
            continue;
        }
        validSamples++;
        sumReturnTemp += data.returnTempC;
        sumSupplyTemp += data.supplyTempC;
        sumDeltaT += data.deltaT;
        sumFanAmps += data.fanAmps;
        sumCompressorAmps += data.compressorAmps;
        sumGeoPumpsAmps += data.geoPumpsAmps;
    }

    AggregatedHVACData result;
    if (validSamples > 0) {
        result.avgReturnTempC = sumReturnTemp / validSamples;
        result.avgSupplyTempC = sumSupplyTemp / validSamples;
        result.avgDeltaT = sumDeltaT / validSamples;
        result.avgFanAmps = sumFanAmps / validSamples;
        result.avgCompressorAmps = sumCompressorAmps / validSamples;
        result.avgGeoPumpsAmps = sumGeoPumpsAmps / validSamples;
    }

    // Capture the final state from the most recent reading, regardless of buffer content
    result.lastFanStatus = lastKnownData.fanStatus;
    result.lastCompressorStatus = lastKnownData.compressorStatus;
    result.lastGeoPumpsStatus = lastKnownData.geoPumpsStatus;

    return result;
}