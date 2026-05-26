// include/utils/Statistics.hpp
#pragma once
#include <map>
#include <string>

class Statistics {
private:
    int totalAlerts;
    int criticalAlerts;
    int warningAlerts;
    std::map<std::string, int> sensorUpdateCount;
    
public:
    Statistics();
    
    void incrementAlert(AlertSeverity severity);
    void incrementSensorUpdate(const std::string& sensorId);
    
    int getTotalAlerts() const;
    int getCriticalAlerts() const;
    int getWarningAlerts() const;
    
    void displayStats() const;
    void reset();
};