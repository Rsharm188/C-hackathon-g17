#include "../../include/utils/Statistics.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

void VehicleStatistics::recordAlert(AlertSeverity sev) {
    ++totalAlerts;
    switch (sev) {
        case AlertSeverity::CRITICAL: ++criticalAlerts; break;
        case AlertSeverity::WARNING:  ++warningAlerts;  break;
        case AlertSeverity::INFO:     ++infoAlerts;     break;
    }
}

void VehicleStatistics::recordSensorUpdate(const std::string& sensorId, double value) {
    ++sensorUpdateCount[sensorId];

    // Track peak (max) — inserts 0.0 on first access via operator[]
    if (sensorPeakValues.find(sensorId) == sensorPeakValues.end()) {
        sensorPeakValues[sensorId] = value;
        sensorMinValues[sensorId]  = value;
    } else {
        sensorPeakValues[sensorId] = std::max(sensorPeakValues[sensorId], value);
        sensorMinValues[sensorId]  = std::min(sensorMinValues[sensorId],  value);
    }
}

int VehicleStatistics::getTotalAlerts()    const { return totalAlerts; }
int VehicleStatistics::getCriticalAlerts() const { return criticalAlerts; }
int VehicleStatistics::getWarningAlerts()  const { return warningAlerts; }
int VehicleStatistics::getInfoAlerts()     const { return infoAlerts; }

void VehicleStatistics::display() const {
    std::cout << "\n  Alerts  — Total: " << totalAlerts
              << "  Critical: "  << criticalAlerts
              << "  Warning: "   << warningAlerts
              << "  Info: "      << infoAlerts << "\n";

    std::cout << "  Sensor updates:\n";
    for (const auto& [id, count] : sensorUpdateCount) {
        std::cout << "    " << std::left << std::setw(12) << id
                  << "  updates: " << count;
        if (sensorPeakValues.count(id)) {
            std::cout << "  peak: " << std::setw(8) << sensorPeakValues.at(id)
                      << "  min: "  << sensorMinValues.at(id);
        }
        std::cout << "\n";
    }
}

void VehicleStatistics::reset() {
    totalAlerts    = criticalAlerts = warningAlerts = infoAlerts = 0;
    sensorUpdateCount.clear();
    sensorPeakValues.clear();
    sensorMinValues.clear();
}