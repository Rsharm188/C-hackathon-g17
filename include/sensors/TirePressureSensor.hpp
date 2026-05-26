#pragma once
#include "core/Sensor.hpp"

class TirePressureSensor : public Sensor {
private:
    double pressure;
public:
    TirePressureSensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};