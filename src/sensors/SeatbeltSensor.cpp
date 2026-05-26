#include "../../include/sensors/SeatbeltSensor.hpp"
#include <iostream>
#include <cstdlib>

SeatbeltSensor::SeatbeltSensor()
    : Sensor("BELT_01", SensorType::SEATBELT_STATUS),
      locked(true)
{}

void SeatbeltSensor::update() {
    // Simulate rare unlocked event (3% chance to toggle)
    if ((std::rand() % 100) < 3)
        locked = !locked;
}

void SeatbeltSensor::display() const { std::cout << *this << "\n"; }

double      SeatbeltSensor::getValue()  const { return locked ? 1.0 : 0.0; }
std::string SeatbeltSensor::getUnit()   const { return ""; }
std::string SeatbeltSensor::getStatus()const { return locked ? "LOCKED" : "UNLOCKED"; }
bool        SeatbeltSensor::isLocked() const { return locked; }