#pragma once
#include "core/Sensor.hpp"

class BatterySensor : public Sensor {
private:
    double voltage;
public:
    BatterySensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};