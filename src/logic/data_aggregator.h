#ifndef DATA_AGGREGATOR_H
#define DATA_AGGREGATOR_H

#include "config.h"
#include "hvac_data.h"
#include <array>

namespace DataAggregator {
    AggregatedHVACData aggregate(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer, const HVACData& lastKnownData);
} // namespace DataAggregator

#endif // DATA_AGGREGATOR_H