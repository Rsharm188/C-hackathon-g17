#include "../../include/managers/SensorManager.hpp"
#include "../../include/sensors/EngineTemperatureSensor.hpp"
#include "../../include/sensors/BatterySensor.hpp"
#include "../../include/sensors/SpeedSensor.hpp"
#include "../../include/sensors/TirePressureSensor.hpp"
#include "../../include/sensors/DoorSensor.hpp"
#include "../../include/sensors/SeatbeltSensor.hpp"
#include <iostream>
#include <stdexcept>

SensorManager::SensorManager(std::shared_ptr<VehicleData>       vd,
                              std::shared_ptr<VehicleStatistics> stats)
    : vehicleData_(std::move(vd)), stats_(std::move(stats))
{}

// ── initialise ─────────────────────────────────────────────────────────────────
// Demonstrates: unique_ptr, Runtime Polymorphism via base-class pointer,
//               STL vector of polymorphic objects
void SensorManager::initialise() {
    sensors_.push_back(std::make_unique<EngineTemperatureSensor>());
    sensors_.push_back(std::make_unique<BatterySensor>());
    sensors_.push_back(std::make_unique<SpeedSensor>());
    sensors_.push_back(std::make_unique<TirePressureSensor>());
    sensors_.push_back(std::make_unique<DoorSensor>());
    sensors_.push_back(std::make_unique<SeatbeltSensor>());
}

// ── updateAll ──────────────────────────────────────────────────────────────────
// Demonstrates: Runtime Polymorphism — virtual dispatch through update()
void SensorManager::updateAll() {
    for (auto& sensor : sensors_) {
        sensor->update();   // virtual call — correct derived method executes
        stats_->recordSensorUpdate(sensor->getId(), sensor->getValue());
    }

    // Push fresh readings into the shared VehicleData store
    // Down-cast only where we need type-specific boolean data
    for (auto& sensor : sensors_) {
        switch (sensor->getType()) {
            case SensorType::ENGINE_TEMPERATURE:
                vehicleData_->setEngineTemp(sensor->getValue());
                break;
            case SensorType::BATTERY_VOLTAGE:
                vehicleData_->setBatteryVoltage(sensor->getValue());
                break;
            case SensorType::VEHICLE_SPEED:
                vehicleData_->setSpeed(sensor->getValue());
                break;
            case SensorType::TIRE_PRESSURE:
                vehicleData_->setTirePressure(sensor->getValue());
                break;
            case SensorType::DOOR_STATUS:
                vehicleData_->setDoorOpen(sensor->getValue() > 0.5);
                break;
            case SensorType::SEATBELT_STATUS:
                vehicleData_->setSeatbeltLocked(sensor->getValue() > 0.5);
                break;
        }
    }
}

// ── displayAll ─────────────────────────────────────────────────────────────────
// Demonstrates: Runtime Polymorphism via display() virtual call + operator<<
void SensorManager::displayAll() const {
    for (const auto& sensor : sensors_) {
        std::cout << *sensor << "\n";   // operator<< calls virtual getValue/getUnit
    }
}

const Sensor& SensorManager::getSensor(size_t index) const {
    if (index >= sensors_.size())
        throw std::out_of_range("SensorManager: index out of range");
    return *sensors_[index];
}

size_t SensorManager::count() const { return sensors_.size(); }