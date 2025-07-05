#ifndef ENUM_CONVERTERS_H
#define ENUM_CONVERTERS_H

#include "hvac_status_types.h"

inline const char* toString(ComponentStatus status) {
    switch (status) {
        case ComponentStatus::ON:  return "ON";
        case ComponentStatus::OFF: return "OFF";
        case ComponentStatus::UNKNOWN: return "UNKNOWN";
        default:                   return "UNKNOWN";
    }
}

inline const char* toString(AirflowStatus status) {
    switch (status) {
        case AirflowStatus::OK:  return "OK";
        case AirflowStatus::NA:  return "N/A";
        default:                 return "UNKNOWN";
    }
}

inline const char* toString(AlertStatus status) {
    switch (status) {
        case AlertStatus::NONE: return "NONE";
        default:                return "UNKNOWN";
    }
}

#endif // ENUM_CONVERTERS_H