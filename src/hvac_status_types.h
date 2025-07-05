#ifndef HVAC_STATUS_TYPES_H
#define HVAC_STATUS_TYPES_H

enum class ComponentStatus { OFF, ON, UNKNOWN };
enum class AirflowStatus { NA, OK };
enum class AlertStatus { NONE, FAN_NO_AIRFLOW, LOW_DELTA_T, TEMP_SENSOR_DISCONNECTED };

#endif // HVAC_STATUS_TYPES_H