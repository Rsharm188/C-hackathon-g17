// include/core/Sensor.hpp
#pragma once
#include <string>
#include <memory>

class Sensor {
protected:
    std::string sensorId;
    std::string sensorType;
    double currentValue;
    bool isActive;
    
public:
    Sensor(const std::string& id, const std::string& type);
    virtual ~Sensor() = default;
    
    // Pure virtual functions
    virtual void update() = 0;
    virtual void display() const = 0;
    virtual double getValue() const = 0;
    virtual std::string getUnit() const = 0;
    
    // Common interface
    std::string getId() const;
    std::string getType() const;
    bool isHealthy() const;
    
    // Operator overloading
    friend std::ostream& operator<<(std::ostream& os, const Sensor& sensor);
};