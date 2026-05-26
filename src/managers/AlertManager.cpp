#include "../../include/managers/AlertManager.hpp"
#include "../../include/core/Types.hpp"
#include <algorithm>
#include <iostream>

AlertManager::AlertManager(std::shared_ptr<VehicleStatistics> stats)
    : stats_(std::move(stats))
{}

// ── raiseAlert ─────────────────────────────────────────────────────────────────
// Only adds the alert if no active alert with the same source + message exists
void AlertManager::raiseAlert(AlertSeverity sev,
                               const std::string& msg,
                               const std::string& source) {
    // Lambda: check for duplicate — demonstrates lambda expressions
    auto isDuplicate = [&](const std::shared_ptr<Alert>& a) {
        return a->isActive() && a->getSource() == source && a->getMessage() == msg;
    };

    auto it = std::find_if(activeAlerts_.begin(), activeAlerts_.end(), isDuplicate);
    if (it != activeAlerts_.end()) return;   // already active, skip

    auto alert = std::make_shared<Alert>(sev, msg, source);
    activeAlerts_.push_back(alert);
    alertHistory_.push_back(alert);          // history keeps all-time records
    stats_->recordAlert(sev);
}

// ── clearAlertBySource ─────────────────────────────────────────────────────────
void AlertManager::clearAlertBySource(const std::string& source) {
    // Lambda to mark matching alerts inactive
    std::for_each(activeAlerts_.begin(), activeAlerts_.end(),
                  [&](std::shared_ptr<Alert>& a) {
                      if (a->getSource() == source) a->deactivate();
                  });

    // STL erase-remove idiom with lambda predicate
    activeAlerts_.erase(
        std::remove_if(activeAlerts_.begin(), activeAlerts_.end(),
                       [](const std::shared_ptr<Alert>& a) { return !a->isActive(); }),
        activeAlerts_.end());
}

// ── evaluate ───────────────────────────────────────────────────────────────────
// Called each monitoring cycle — checks all conditions
void AlertManager::evaluate(const VehicleData& data) {
    std::lock_guard<std::mutex> lock(alertMutex_);

    // Read a consistent snapshot (VehicleData is internally thread-safe)
    const double temp     = data.getEngineTemp();
    const double voltage  = data.getBatteryVoltage();
    const double speed    = data.getSpeed();
    const double pressure = data.getTirePressure();
    const bool   door     = data.isDoorOpen();
    const bool   belt     = data.isSeatbeltLocked();

    // ── Engine Temperature ────────────────────────────────────────────────────
    if (temp > Thresholds::ENGINE_TEMP_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "CRITICAL ENGINE OVERHEAT", "ENG_TEMP_01");
    else if (temp > Thresholds::ENGINE_TEMP_WARNING)
        raiseAlert(AlertSeverity::WARNING, "ENGINE TEMPERATURE HIGH", "ENG_TEMP_01");
    else
        clearAlertBySource("ENG_TEMP_01");

    // ── Battery Voltage ───────────────────────────────────────────────────────
    if (voltage < Thresholds::BATTERY_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW BATTERY WARNING", "BAT_01");
    else if (voltage < Thresholds::BATTERY_WARNING)
        raiseAlert(AlertSeverity::WARNING, "BATTERY VOLTAGE LOW", "BAT_01");
    else
        clearAlertBySource("BAT_01");

    // ── Speed ─────────────────────────────────────────────────────────────────
    if (speed > Thresholds::SPEED_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "OVERSPEED WARNING", "SPD_01");
    else
        clearAlertBySource("SPD_01");

    // ── Tire Pressure ─────────────────────────────────────────────────────────
    if (pressure < Thresholds::TIRE_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW TIRE PRESSURE", "TIRE_01");
    else if (pressure < Thresholds::TIRE_WARNING)
        raiseAlert(AlertSeverity::WARNING, "TIRE PRESSURE WARNING", "TIRE_01");
    else
        clearAlertBySource("TIRE_01");

    // ── Door while moving ─────────────────────────────────────────────────────
    if (door && speed > Thresholds::SPEED_DOOR_WARN)
        raiseAlert(AlertSeverity::WARNING, "DOOR OPEN WARNING", "DOOR_01");
    else
        clearAlertBySource("DOOR_01");

    // ── Seatbelt while moving ─────────────────────────────────────────────────
    if (!belt && speed > 0.0)
        raiseAlert(AlertSeverity::WARNING, "SEATBELT WARNING", "BELT_01");
    else
        clearAlertBySource("BELT_01");
}

// ── Thread-safe accessors ──────────────────────────────────────────────────────
std::vector<std::shared_ptr<Alert>> AlertManager::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return activeAlerts_;   // returns a copy — safe for caller
}

std::vector<std::shared_ptr<Alert>> AlertManager::getAlertHistory() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return alertHistory_;
}

// ── filterActive — uses std::copy_if + lambda ─────────────────────────────────
// Demonstrates: lambda expressions, STL algorithms
std::vector<std::shared_ptr<Alert>>
AlertManager::filterActive(std::function<bool(const Alert&)> predicate) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;
    std::copy_if(activeAlerts_.begin(), activeAlerts_.end(),
                 std::back_inserter(result),
                 [&](const std::shared_ptr<Alert>& a) { return predicate(*a); });
    return result;
}

// ── searchHistory — uses std::find_if + lambda ────────────────────────────────
std::vector<std::shared_ptr<Alert>>
AlertManager::searchHistory(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;
    std::copy_if(alertHistory_.begin(), alertHistory_.end(),
                 std::back_inserter(result),
                 // Lambda: checks if keyword appears in message
                 [&](const std::shared_ptr<Alert>& a) {
                     return a->getMessage().find(keyword) != std::string::npos;
                 });
    return result;
}

// ── getBySeverity — wraps filterActive with a severity lambda ─────────────────
std::vector<std::shared_ptr<Alert>>
AlertManager::getBySeverity(AlertSeverity sev) const {
    return filterActive([sev](const Alert& a) { return a.getSeverity() == sev; });
}

int AlertManager::activeCount()  const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(activeAlerts_.size());
}
int AlertManager::historyCount() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(alertHistory_.size());
}