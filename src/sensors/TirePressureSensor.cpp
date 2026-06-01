#include "../../include/sensors/TirePressureSensor.hpp"
#include "../../include/core/Types.hpp"
#include <iostream>
#include <cstdlib>

TirePressureSensor::TirePressureSensor()
    : Sensor("TIRE_01", SensorType::TIRE_PRESSURE), pressure(30.0) {}

void TirePressureSensor::update() {
    double delta = (std::rand() % 5 - 2) * 0.2;
    pressure += delta;
    if (pressure < Thresholds::TIRE_MIN) pressure = Thresholds::TIRE_MIN;
    if (pressure > Thresholds::TIRE_MAX) pressure = Thresholds::TIRE_MAX;
}

void        TirePressureSensor::display()   const { std::cout << *this << "\n"; }
double      TirePressureSensor::getValue()  const { return pressure; }
std::string TirePressureSensor::getUnit()   const { return "PSI"; }
std::string TirePressureSensor::getStatus() const {
    if (pressure < Thresholds::TIRE_LOW)     return "CRITICAL";
    if (pressure < Thresholds::TIRE_WARNING) return "WARNING";
    return "NORMAL";
}

// ── Mock ──────────────────────────────────────────────────────────────────────
MockTirePressureSensor::MockTirePressureSensor()
    : Sensor("TIRE_01", SensorType::TIRE_PRESSURE) {}

void        MockTirePressureSensor::display()   const { std::cout << *this << "\n"; }
double      MockTirePressureSensor::getValue()  const { return pressure; }
std::string MockTirePressureSensor::getUnit()   const { return "PSI"; }
std::string MockTirePressureSensor::getStatus() const {
    if (pressure < Thresholds::TIRE_LOW)     return "CRITICAL";
    if (pressure < Thresholds::TIRE_WARNING) return "WARNING";
    return "NORMAL";
}