#pragma once
#include "core/Sensor.hpp"
#include "core/Types.hpp"

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

class MockSpeedSensor : public Sensor {
private:
    double speed = 0.0;
public:
    MockSpeedSensor();
    void        setValue(double val) { speed = val; }
    void        update()    override {}
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};