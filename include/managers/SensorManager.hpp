// include/managers/SensorManager.hpp
#pragma once
#include <vector>
#include <memory>
#include "core/Sensor.hpp"
#include "core/VehicleData.hpp"
#include "utils/Statistics.hpp"

class SensorManager {
private:
    std::vector<std::unique_ptr<Sensor>> sensors_;
    std::shared_ptr<VehicleData>         vehicleData_;
    std::shared_ptr<VehicleStatistics>   stats_;

public:
    SensorManager(std::shared_ptr<VehicleData>       vd,
                  std::shared_ptr<VehicleStatistics> stats);

    void initialise();
    void updateAll();
    void displayAll() const;

    const Sensor& getSensor(size_t index) const;
    size_t        count()                 const;
};