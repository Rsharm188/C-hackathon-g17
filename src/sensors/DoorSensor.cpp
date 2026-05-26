#include "../../include/sensors/DoorSensor.hpp"
#include <iostream>
#include <cstdlib>

DoorSensor::DoorSensor()
    : Sensor("DOOR_01", SensorType::DOOR_STATUS),
      open(false)
{}

void DoorSensor::update() {
    // Simulate occasional random door event (5% chance to toggle)
    if ((std::rand() % 100) < 5)
        open = !open;
}

void DoorSensor::display() const { std::cout << *this << "\n"; }

double      DoorSensor::getValue()  const { return open ? 1.0 : 0.0; }
std::string DoorSensor::getUnit()   const { return ""; }
std::string DoorSensor::getStatus() const { return open ? "OPEN" : "CLOSED"; }
bool        DoorSensor::isOpen()    const { return open; }