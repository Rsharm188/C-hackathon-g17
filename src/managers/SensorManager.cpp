#include "../../include/managers/SensorManager.hpp"
#include <iostream>
#include <stdexcept>

SensorManager::SensorManager(std::shared_ptr<VehicleData>       vd,
                              std::shared_ptr<VehicleStatistics> stats)
    : vehicleData_(std::move(vd)), stats_(std::move(stats))
{}

void SensorManager::initialise() {
    testMode_ = false;
    sensors_.push_back(std::make_unique<EngineTemperatureSensor>());
    sensors_.push_back(std::make_unique<BatterySensor>());
    sensors_.push_back(std::make_unique<SpeedSensor>());
    sensors_.push_back(std::make_unique<TirePressureSensor>());
    sensors_.push_back(std::make_unique<DoorSensor>());
    sensors_.push_back(std::make_unique<SeatbeltSensor>());
}

void SensorManager::initialiseForTest() {
    testMode_ = true;
    sensors_.clear();

    auto temp    = std::make_unique<MockEngineTemperatureSensor>();
    auto battery = std::make_unique<MockBatterySensor>();
    auto speed   = std::make_unique<MockSpeedSensor>();
    auto tyre    = std::make_unique<MockTirePressureSensor>();
    auto door    = std::make_unique<MockDoorSensor>();
    auto belt    = std::make_unique<MockSeatbeltSensor>();

    // Keep raw pointers for injection (safe — unique_ptr owns them)
    mockTemp_    = temp.get();
    mockBattery_ = battery.get();
    mockSpeed_   = speed.get();
    mockTyre_    = tyre.get();
    mockDoor_    = door.get();
    mockBelt_    = belt.get();

    sensors_.push_back(std::move(temp));
    sensors_.push_back(std::move(battery));
    sensors_.push_back(std::move(speed));
    sensors_.push_back(std::move(tyre));
    sensors_.push_back(std::move(door));
    sensors_.push_back(std::move(belt));
}

void SensorManager::injectTestCase(double engineTemp, double batteryVoltage,
                                    double speed,      double tirePressure,
                                    bool   doorOpen,   bool   seatbeltLocked) {
    if (!testMode_) return;
    mockTemp_   ->setValue(engineTemp);
    mockBattery_->setValue(batteryVoltage);
    mockSpeed_  ->setValue(speed);
    mockTyre_   ->setValue(tirePressure);
    mockDoor_   ->setValue(doorOpen);
    mockBelt_   ->setValue(seatbeltLocked);
}

void SensorManager::updateAll() {
    for (auto& sensor : sensors_) {
        sensor->update();
        stats_->recordSensorUpdate(sensor->getId(), sensor->getValue());
    }

    for (auto& sensor : sensors_) {
        switch (sensor->getType()) {
            case SensorType::ENGINE_TEMPERATURE:
                vehicleData_->setEngineTemp(sensor->getValue()); break;
            case SensorType::BATTERY_VOLTAGE:
                vehicleData_->setBatteryVoltage(sensor->getValue()); break;
            case SensorType::VEHICLE_SPEED:
                vehicleData_->setSpeed(sensor->getValue()); break;
            case SensorType::TIRE_PRESSURE:
                vehicleData_->setTirePressure(sensor->getValue()); break;
            case SensorType::DOOR_STATUS:
                vehicleData_->setDoorOpen(sensor->getValue() > 0.5); break;
            case SensorType::SEATBELT_STATUS:
                vehicleData_->setSeatbeltLocked(sensor->getValue() > 0.5); break;
        }
    }
}

void SensorManager::displayAll() const {
    for (const auto& sensor : sensors_)
        std::cout << *sensor << "\n";
}

const Sensor& SensorManager::getSensor(size_t index) const {
    if (index >= sensors_.size())
        throw std::out_of_range("SensorManager: index out of range");
    return *sensors_[index];
}

size_t SensorManager::count() const { return sensors_.size(); }