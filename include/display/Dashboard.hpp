// include/display/Dashboard.hpp
#pragma once
#include <memory>
#include <string>
#include <ostream>
#include "core/VehicleData.hpp"
#include "managers/AlertManager.hpp"
#include "managers/SensorManager.hpp"
#include "utils/Statistics.hpp"

class Dashboard {
private:
    std::shared_ptr<VehicleData>       vehicleData_;
    std::shared_ptr<AlertManager>      alertManager_;
    std::shared_ptr<SensorManager>     sensorManager_;
    std::shared_ptr<VehicleStatistics> stats_;

    void printSeparator(char c = '-', int width = 60) const;
    void printHeader()      const;
    void printFooter()      const;
    void printSensorPanel() const;
    void printAlertPanel()  const;
    void printStatsPanel()  const;

public:
    Dashboard(std::shared_ptr<VehicleData>       vd,
              std::shared_ptr<AlertManager>      am,
              std::shared_ptr<SensorManager>     sm,
              std::shared_ptr<VehicleStatistics> stats);

    void render()                               const;
    void exportSnapshot(const std::string& filename) const;

    friend std::ostream& operator<<(std::ostream& os, const Dashboard& db);
};