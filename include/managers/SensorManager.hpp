#pragma once
#include <vector>
#include <memory>
#include "core/Sensor.hpp"
#include "core/VehicleData.hpp"
#include "utils/Statistics.hpp"
#include "sensors/EngineTemperatureSensor.hpp"
#include "sensors/BatterySensor.hpp"
#include "sensors/SpeedSensor.hpp"
#include "sensors/TirePressureSensor.hpp"
#include "sensors/DoorSensor.hpp"
#include "sensors/SeatbeltSensor.hpp"

class SensorManager {
private:
    std::vector<std::unique_ptr<Sensor>> sensors_;
    std::shared_ptr<VehicleData>         vehicleData_;
    std::shared_ptr<VehicleStatistics>   stats_;

    // Mock sensor pointers for test mode
    MockEngineTemperatureSensor* mockTemp_     = nullptr;
    MockBatterySensor*           mockBattery_  = nullptr;
    MockSpeedSensor*             mockSpeed_    = nullptr;
    MockTirePressureSensor*      mockTyre_     = nullptr;
    MockDoorSensor*              mockDoor_     = nullptr;
    MockSeatbeltSensor*          mockBelt_     = nullptr;

    bool testMode_ = false;

public:
    SensorManager(std::shared_ptr<VehicleData>       vd,
                  std::shared_ptr<VehicleStatistics> stats);

    void initialise();           // Mode 0 — real sensors
    void initialiseForTest();    // Mode 1 — mock sensors

    void injectTestCase(double engineTemp, double batteryVoltage,
                        double speed,      double tirePressure,
                        bool   doorOpen,   bool   seatbeltLocked);

    void updateAll();
    void displayAll() const;

    const Sensor& getSensor(size_t index) const;
    size_t        count() const;
};