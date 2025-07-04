#include "emon_lib_adapter.h"

EmonLibAdapter::EmonLibAdapter(EnergyMonitor& monitor)
    : _monitor(monitor) {}

double EmonLibAdapter::calcIrms(unsigned int samples) {
    return _monitor.calcIrms(samples);
}