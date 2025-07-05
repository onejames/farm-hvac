#ifndef DATA_AGGREGATOR_H
#define DATA_AGGREGATOR_H

#include "hvac_data.h"
#include "config.h"
#include <array>

class DataAggregator {
public:
    static AggregatedHVACData aggregate(const std::array<HVACData, DATA_BUFFER_SIZE>& dataBuffer);
};

#endif // DATA_AGGREGATOR_H