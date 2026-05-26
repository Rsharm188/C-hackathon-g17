#pragma once
#include "core/Sensor.hpp"

class SeatbeltSensor : public Sensor {
private:
    bool locked;
public:
    SeatbeltSensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
    bool        isLocked()  const;
};