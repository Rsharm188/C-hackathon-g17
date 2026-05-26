#include "../../include/core/Sensor.hpp"
#include <iostream>
#include <iomanip>

// ── Static member definition ───────────────────────────────────────────────────
int Sensor::instanceCount = 0;

// ── Constructor / Destructor ───────────────────────────────────────────────────
Sensor::Sensor(const std::string& id, SensorType type)
    : sensorId(id), sensorType(type), isActive(true)
{
    ++instanceCount;
}

Sensor::~Sensor() {
    --instanceCount;
}

// ── Concrete method implementations ───────────────────────────────────────────
std::string Sensor::getId()       const { return sensorId; }
SensorType  Sensor::getType()     const { return sensorType; }
std::string Sensor::getTypeName() const { return sensorTypeToString(sensorType); }
bool        Sensor::getIsActive() const { return isActive; }
void        Sensor::setActive(bool active) { isActive = active; }

int Sensor::getTotalInstances() { return instanceCount; }

// ── Operator << ────────────────────────────────────────────────────────────────
// Demonstrates: Operator Overloading + Runtime Polymorphism (calls virtual getValue/getUnit)
std::ostream& operator<<(std::ostream& os, const Sensor& s) {
    os << std::left << std::setw(25) << s.getTypeName()
       << " | ID: " << std::setw(10) << s.getId()
       << " | Value: " << std::setw(8) << s.getValue()
       << " " << s.getUnit()
       << " | Status: " << s.getStatus();
    return os;
}