#ifndef DATA_ANALYZER_H
#define DATA_ANALYZER_H

// Forward declaration to avoid including the full header
struct HVACData;

class DataAnalyzer {
public:
    static void process(HVACData& data, float ampsOnThreshold);
};

#endif // DATA_ANALYZER_H