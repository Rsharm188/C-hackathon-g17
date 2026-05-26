#include "../../include/sensors/SpeedSensor.hpp"
#include "../../include/core/Types.hpp"
#include <iostream>
#include <cstdlib>

SpeedSensor::SpeedSensor()
    : Sensor("SPD_01", SensorType::VEHICLE_SPEED),
      speed(0.0)
{}

void SpeedSensor::update() {
    double delta = (std::rand() % 15 - 5) * 1.0;   // -5 to +9 km/h per tick
    speed += delta;
    if (speed < Thresholds::SPEED_MIN) speed = Thresholds::SPEED_MIN;
    if (speed > Thresholds::SPEED_MAX) speed = Thresholds::SPEED_MAX;
}

void SpeedSensor::display() const { std::cout << *this << "\n"; }

double      SpeedSensor::getValue()  const { return speed; }
std::string SpeedSensor::getUnit()   const { return "km/h"; }
std::string SpeedSensor::getStatus() const {
    if (speed > Thresholds::SPEED_CRITICAL) return "OVERSPEED";
    if (speed > 0.0)                        return "MOVING";
    return "STOPPED";
}