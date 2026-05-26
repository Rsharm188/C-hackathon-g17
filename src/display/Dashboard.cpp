#include "../../include/display/Dashboard.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdexcept>

Dashboard::Dashboard(std::shared_ptr<VehicleData>       vd,
                     std::shared_ptr<AlertManager>      am,
                     std::shared_ptr<SensorManager>     sm,
                     std::shared_ptr<VehicleStatistics> stats)
    : vehicleData_(std::move(vd)),
      alertManager_(std::move(am)),
      sensorManager_(std::move(sm)),
      stats_(std::move(stats))
{}

// ── Helpers ────────────────────────────────────────────────────────────────────
void Dashboard::printSeparator(char c, int width) const {
    std::cout << std::string(width, c) << "\n";
}

void Dashboard::printHeader() const {
    printSeparator('=');
    std::cout << "  SMART CABIN & VEHICLE HEALTH MONITOR  |  "
              << currentTimestamp() << "\n";
    printSeparator('=');
}

void Dashboard::printFooter() const {
    printSeparator('-');
    std::cout << "  Total Sensors: "   << sensorManager_->count()
              << "  |  Active Alerts: " << alertManager_->activeCount()
              << "  |  All-time Alerts: " << Alert::getTotalAlerts()
              << "  |  Sensor Instances: " << Sensor::getTotalInstances()
              << "\n";
    printSeparator('=');
}

// ── Sensor Panel ────────────────────────────────────────────────────────────────
void Dashboard::printSensorPanel() const {
    std::cout << "\n  [ SENSORS ]\n";
    printSeparator('-');
    // operator<< on each Sensor — runtime polymorphism + operator overloading
    sensorManager_->displayAll();
}

// ── Alert Panel ─────────────────────────────────────────────────────────────────
void Dashboard::printAlertPanel() const {
    std::cout << "\n  [ ACTIVE ALERTS ]\n";
    printSeparator('-');

    auto alerts = alertManager_->getActiveAlerts();
    if (alerts.empty()) {
        std::cout << "  No active alerts.\n";
        return;
    }
    for (const auto& alert : alerts) {
        std::cout << "  " << *alert << "\n";   // operator<< on Alert
    }
}

// ── Stats Panel ─────────────────────────────────────────────────────────────────
void Dashboard::printStatsPanel() const {
    std::cout << "\n  [ STATISTICS ]\n";
    printSeparator('-');
    stats_->display();
}

// ── Full render ─────────────────────────────────────────────────────────────────
void Dashboard::render() const {
    // Clear terminal
    std::cout << "\033[2J\033[1;1H";

    printHeader();
    printSensorPanel();
    printAlertPanel();
    printStatsPanel();
    printFooter();
}

// ── File export — demonstrates: file streams ────────────────────────────────────
void Dashboard::exportSnapshot(const std::string& filename) const {
    // Demonstrates: Exception Handling around file I/O
    try {
        std::ofstream out(filename, std::ios::out | std::ios::app);
        if (!out.is_open())
            throw std::runtime_error("Cannot open snapshot file: " + filename);

        out << "\n========================================\n";
        out << "SNAPSHOT — " << currentTimestamp() << "\n";
        out << "========================================\n";

        // Sensor data
        auto snap = vehicleData_->snapshot();
        out << "SENSOR READINGS:\n";
        for (const auto& [key, val] : snap)
            out << "  " << std::left << std::setw(18) << key << ": " << val << "\n";

        // Active alerts
        out << "ACTIVE ALERTS:\n";
        for (const auto& alert : alertManager_->getActiveAlerts()) {
            std::ostringstream oss;
            oss << *alert;
            out << "  " << oss.str() << "\n";
        }

        out << "Stats — Total alerts: " << Alert::getTotalAlerts() << "\n";
        out.flush();

    } catch (const std::exception& e) {
        std::cerr << "[Dashboard] Export failed: " << e.what() << "\n";
    }
}

// ── operator<< — compact one-line summary ─────────────────────────────────────
// Demonstrates: stream operators (<<)
std::ostream& operator<<(std::ostream& os, const Dashboard& db) {
    os << "[Dashboard | "  << currentTimestamp()
       << " | Alerts: "   << db.alertManager_->activeCount()
       << " | Sensors: "  << db.sensorManager_->count()
       << " | Speed: "    << std::fixed << std::setprecision(1)
                           << db.vehicleData_->getSpeed()  << " km/h"
       << " | Temp: "     << db.vehicleData_->getEngineTemp() << " C"
       << "]";
    return os;
}