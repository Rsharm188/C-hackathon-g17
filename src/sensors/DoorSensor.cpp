#include "../../include/sensors/DoorSensor.hpp"
#include <iostream>
#include <cstdlib>

DoorSensor::DoorSensor()
    : Sensor("DOOR_01", SensorType::DOOR_STATUS), open(false) {}

void DoorSensor::update() {
    if ((std::rand() % 100) < 5) open = !open;
}

void        DoorSensor::display()   const { std::cout << *this << "\n"; }
double      DoorSensor::getValue()  const { return open ? 1.0 : 0.0; }
std::string DoorSensor::getUnit()   const { return ""; }
std::string DoorSensor::getStatus() const { return open ? "OPEN" : "CLOSED"; }
bool        DoorSensor::isOpen()    const { return open; }

// ── Mock ──────────────────────────────────────────────────────────────────────
MockDoorSensor::MockDoorSensor()
    : Sensor("DOOR_01", SensorType::DOOR_STATUS) {}

void        MockDoorSensor::display()   const { std::cout << *this << "\n"; }
double      MockDoorSensor::getValue()  const { return open ? 1.0 : 0.0; }
std::string MockDoorSensor::getUnit()   const { return ""; }
std::string MockDoorSensor::getStatus() const { return open ? "OPEN" : "CLOSED"; }
bool        MockDoorSensor::isOpen()    const { return open; }