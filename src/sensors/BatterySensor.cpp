#include "../../include/sensors/BatterySensor.hpp"
#include "../../include/core/Types.hpp"
#include <iostream>
#include <cstdlib>

BatterySensor::BatterySensor()
    : Sensor("BAT_01", SensorType::BATTERY_VOLTAGE),
      voltage(12.5)
{}

void BatterySensor::update() {
    double delta = (std::rand() % 7 - 3) * 0.1;   // -0.3 to +0.3 V
    voltage += delta;
    if (voltage < Thresholds::BATTERY_MIN) voltage = Thresholds::BATTERY_MIN;
    if (voltage > Thresholds::BATTERY_MAX) voltage = Thresholds::BATTERY_MAX;
}

void BatterySensor::display() const { std::cout << *this << "\n"; }

double      BatterySensor::getValue()  const { return voltage; }
std::string BatterySensor::getUnit()   const { return "V"; }
std::string BatterySensor::getStatus() const {
    if (voltage < Thresholds::BATTERY_LOW)     return "CRITICAL";
    if (voltage < Thresholds::BATTERY_WARNING) return "WARNING";
    return "NORMAL";
}