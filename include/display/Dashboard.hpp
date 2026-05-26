// include/display/Dashboard.hpp
#pragma once
#include <memory>
#include <vector>
#include "VehicleData.hpp"
#include "AlertManager.hpp"
#include "Sensor.hpp"
#include "Statistics.hpp"

class Dashboard {
private:
    std::shared_ptr<VehicleData> vehicleData;
    std::shared_ptr<AlertManager> alertManager;
    std::vector<std::shared_ptr<Sensor>> sensors;
    std::unique_ptr<Statistics> stats;
    
    void displayHeader() const;
    void displaySensorValues() const;
    void displayActiveAlerts() const;
    void displayStatistics() const;
    
public:
    Dashboard(std::shared_ptr<VehicleData> data,
              std::shared_ptr<AlertManager> alertMgr,
              const std::vector<std::shared_ptr<Sensor>>& sensorList);
    
    ~Dashboard();
    
    void render() const;
    void exportToFile(const std::string& filename) const;
    
    // Stream operator
    friend std::ostream& operator<<(std::ostream& os, const Dashboard& dashboard);
};