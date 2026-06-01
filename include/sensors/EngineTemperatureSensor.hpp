#pragma once
#include "core/Sensor.hpp"
#include "core/Types.hpp"

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

class MockEngineTemperatureSensor : public Sensor {
private:
    double temperature = 85.0;
public:
    MockEngineTemperatureSensor();
    void        setValue(double val) { temperature = val; }
    void        update()    override {} // no-op
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
};