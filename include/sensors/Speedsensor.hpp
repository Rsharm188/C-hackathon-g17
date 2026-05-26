#pragma once
#include "core/Sensor.hpp"

class SpeedSensor : public Sensor {
private:
    double speed;
public:
    SpeedSensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};