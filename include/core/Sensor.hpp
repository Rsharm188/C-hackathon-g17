// include/core/Sensor.hpp
#pragma once
#include <string>
#include <ostream>

enum class SensorType {
    ENGINE_TEMPERATURE,
    BATTERY_VOLTAGE,
    VEHICLE_SPEED,
    TIRE_PRESSURE,
    DOOR_STATUS,
    SEATBELT_STATUS
};

inline std::string sensorTypeToString(SensorType t) {
    switch (t) {
        case SensorType::ENGINE_TEMPERATURE: return "Engine Temperature";
        case SensorType::BATTERY_VOLTAGE:    return "Battery Voltage";
        case SensorType::VEHICLE_SPEED:      return "Vehicle Speed";
        case SensorType::TIRE_PRESSURE:      return "Tire Pressure";
        case SensorType::DOOR_STATUS:        return "Door Status";
        case SensorType::SEATBELT_STATUS:    return "Seatbelt Status";
    }
    return "Unknown";
}

class Sensor {
protected:
    std::string sensorId;
    SensorType  sensorType;
    bool        isActive;

public:
    Sensor(const std::string& id, SensorType type);
    virtual ~Sensor();

    virtual void        update()    = 0;
    virtual void        display()   const = 0;
    virtual double      getValue()  const = 0;
    virtual std::string getUnit()   const = 0;
    virtual std::string getStatus() const = 0;

    std::string getId()        const;
    SensorType  getType()      const;
    std::string getTypeName()  const;
    bool        getIsActive()  const;
    void        setActive(bool active);

    static int getTotalInstances();

    friend std::ostream& operator<<(std::ostream& os, const Sensor& sensor);

private:
    static int instanceCount;
};