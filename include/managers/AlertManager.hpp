// include/managers/AlertManager.hpp
#pragma once
#include <vector>
#include <set>
#include <memory>
#include <mutex>
#include "Alert.hpp"
#include "VehicleData.hpp"

class AlertManager {
private:
    std::vector<std::shared_ptr<Alert>> activeAlerts;
    std::set<std::shared_ptr<Alert>> alertHistory;
    std::mutex alertMutex;
    
    // Alert condition checking methods
    bool checkEngineTemperature(double temp);
    bool checkBatteryVoltage(double voltage);
    bool checkSpeed(double speed);
    bool checkTirePressure(double pressure);
    bool checkDoorStatus(bool isOpen, double speed);
    bool checkSeatbelt(bool isLocked, double speed);
    
public:
    AlertManager();
    ~AlertManager();
    
    void evaluateConditions(const VehicleData& data);
    void addAlert(std::shared_ptr<Alert> alert);
    void clearAlert(int alertId);
    
    std::vector<std::shared_ptr<Alert>> getActiveAlerts() const;
    
    // Lambda usage for filtering
    std::vector<std::shared_ptr<Alert>> filterBySevirity(AlertSeverity sev) const;
    std::vector<std::shared_ptr<Alert>> searchHistory(const std::string& keyword) const;
};