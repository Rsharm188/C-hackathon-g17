#include "../../include/sensors/EngineTemperatureSensor.hpp"
#include "../../include/core/Types.hpp"
#include <iostream>
#include <cstdlib>

// ─── EngineTemperatureSensor ───────────────────────────────────────────────────
EngineTemperatureSensor::EngineTemperatureSensor()
    : Sensor("ENG_TEMP_01", SensorType::ENGINE_TEMPERATURE),
      temperature(85.0)
{}

void EngineTemperatureSensor::update() {
    // Simulated random walk within plausible automotive range
    double delta = (std::rand() % 11 - 4) * 0.5;   // -2.0 to +3.0 °C
    temperature += delta;
    // Clamp
    if (temperature < Thresholds::ENGINE_TEMP_MIN) temperature = Thresholds::ENGINE_TEMP_MIN;
    if (temperature > Thresholds::ENGINE_TEMP_MAX) temperature = Thresholds::ENGINE_TEMP_MAX;
}

void EngineTemperatureSensor::display() const {
    std::cout << *this << "\n";
}

double      EngineTemperatureSensor::getValue()  const { return temperature; }
std::string EngineTemperatureSensor::getUnit()   const { return "C"; }
std::string EngineTemperatureSensor::getStatus() const {
    if (temperature > Thresholds::ENGINE_TEMP_CRITICAL)  return "CRITICAL";
    if (temperature > Thresholds::ENGINE_TEMP_WARNING)   return "WARNING";
    return "NORMAL";
}