#ifndef I_HARDWARE_MANAGER_H
#define I_HARDWARE_MANAGER_H

// Forward declare interfaces to avoid circular dependencies
class ITemperatureSensor;
class ICurrentSensor;

class IHardwareManager {
public:
    virtual ~IHardwareManager() = default;

    virtual void setup() = 0;

    [[nodiscard]] virtual ITemperatureSensor& getTempAdapter() = 0;
    [[nodiscard]] virtual ICurrentSensor& getFanAdapter() = 0;
    [[nodiscard]] virtual ICurrentSensor& getCompressorAdapter() = 0;
    [[nodiscard]] virtual ICurrentSensor& getPumpsAdapter() = 0;
};

#endif // I_HARDWARE_MANAGER_H