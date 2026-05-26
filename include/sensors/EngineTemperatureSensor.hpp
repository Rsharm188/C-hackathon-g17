#pragma once
#include "core/Sensor.hpp"

class EngineTemperatureSensor : public Sensor {
private:
    double temperature;
public:
    EngineTemperatureSensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};