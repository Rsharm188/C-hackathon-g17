#pragma once
#include "core/Sensor.hpp"

class DoorSensor : public Sensor {
private:
    bool open;
public:
    DoorSensor();
    void        update()    override;
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
    bool        isOpen()    const;
};

class MockDoorSensor : public Sensor {
private:
    bool open = false;
public:
    MockDoorSensor();
    void        setValue(bool val) { open = val; }
    void        update()    override {}
    void        display()   const override;
    double      getValue()  const override;
    std::string getUnit()   const override;
    std::string getStatus() const override;
    bool        isOpen()    const;
};