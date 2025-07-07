#include "emon_lib_adapter.h"

#ifdef ARDUINO
// This constructor and method are only for the Arduino build
EmonLibAdapter::EmonLibAdapter(EnergyMonitor& monitor)
    : _monitor(monitor) {}

double EmonLibAdapter::calcIrms(unsigned int samples) {
    return _monitor.calcIrms(samples);
}
#else
// These are the "hollow" implementations for the native build environment.
EmonLibAdapter::EmonLibAdapter() {}

double EmonLibAdapter::calcIrms(unsigned int /*samples*/) {
    // For native tests, return a sensible default or mock value.
    return 0.0;
}
#endif