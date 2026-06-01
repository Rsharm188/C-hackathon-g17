#pragma once
#include "core/Sensor.hpp"
#include "core/Types.hpp"

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

class MockTirePressureSensor : public Sensor {
private:
    double pressure = 30.0;
public:
    MockTirePressureSensor();
    void        setValue(double val) { pressure = val; }
    void        update()    override {}
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};