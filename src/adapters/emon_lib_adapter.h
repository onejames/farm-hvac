#ifndef EMON_LIB_ADAPTER_H
#define EMON_LIB_ADAPTER_H

#ifdef ARDUINO
#include <EmonLib.h> // Hardware-specific library
#endif
#include "interfaces/i_current_sensor.h"

class EmonLibAdapter : public ICurrentSensor {
public:
#ifdef ARDUINO
    explicit EmonLibAdapter(EnergyMonitor& monitor);
#else
    EmonLibAdapter(); // Default constructor for native builds
#endif
    double calcIrms(unsigned int numberOfSamples) override;

private:
#ifdef ARDUINO
    EnergyMonitor& _monitor;
#endif
};

#endif // EMON_LIB_ADAPTER_H