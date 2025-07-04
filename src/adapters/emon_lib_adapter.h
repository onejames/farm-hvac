#ifndef EMON_LIB_ADAPTER_H
#define EMON_LIB_ADAPTER_H

#include "interfaces/i_current_sensor.h"
#include <EmonLib.h>

class EmonLibAdapter : public ICurrentSensor {
public:
    explicit EmonLibAdapter(EnergyMonitor& monitor);
    double calcIrms(unsigned int numberOfSamples) override;

private:
    EnergyMonitor& _monitor;
};

#endif // EMON_LIB_ADAPTER_H