#ifndef I_CURRENT_SENSOR_H
#define I_CURRENT_SENSOR_H

class ICurrentSensor {
public:
    virtual double calcIrms(unsigned int samples) = 0;
};
#endif // I_CURRENT_SENSOR_H