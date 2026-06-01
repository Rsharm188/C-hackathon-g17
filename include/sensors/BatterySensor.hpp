#pragma once
#include "core/Sensor.hpp"
#include "core/Types.hpp"

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

class MockBatterySensor : public Sensor {
private:
    double voltage = 12.5;
public:
    MockBatterySensor();
    void        setValue(double val) { voltage = val; }
    void        update()    override {}
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};