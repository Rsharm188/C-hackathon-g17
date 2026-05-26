// include/utils/Statistics.hpp
#pragma once
#include <map>
#include <string>
#include "core/Alert.hpp"   // for AlertSeverity

class VehicleStatistics {
private:
    int totalAlerts    = 0;
    int criticalAlerts = 0;
    int warningAlerts  = 0;
    int infoAlerts     = 0;

    std::map<std::string, int>    sensorUpdateCount;
    std::map<std::string, double> sensorPeakValues;
    std::map<std::string, double> sensorMinValues;

public:
    void recordAlert(AlertSeverity sev);
    void recordSensorUpdate(const std::string& sensorId, double value);

    int getTotalAlerts()    const;
    int getCriticalAlerts() const;
    int getWarningAlerts()  const;
    int getInfoAlerts()     const;

    void display() const;
    void reset();
};

// Dashboard.hpp uses 'Statistics' so alias it
using Statistics = VehicleStatistics;