#include "../../include/managers/AlertManager.hpp"
#include "../../include/core/Types.hpp"
#include <algorithm>
#include <iostream>

AlertManager::AlertManager(std::shared_ptr<VehicleStatistics> stats,
                           std::shared_ptr<EventLogger>       logger)
    : stats_(std::move(stats)), logger_(std::move(logger))
{}

void AlertManager::raiseAlert(AlertSeverity sev,
                               const std::string& msg,
                               const std::string& source) {
    auto isDuplicate = [&](const std::shared_ptr<Alert>& a) {
        return a->isActive() && a->getSource() == source && a->getMessage() == msg;
    };

    auto it = std::find_if(activeAlerts_.begin(), activeAlerts_.end(), isDuplicate);

    if (it != activeAlerts_.end()) {
        // Alert already active — check if 5 seconds passed for reminder
        auto now = std::chrono::steady_clock::now();
        auto key = source + ":" + msg;
        auto lastIt = lastLoggedTime_.find(key);

        if (lastIt == lastLoggedTime_.end() ||
            std::chrono::duration_cast<std::chrono::seconds>(now - lastIt->second).count() >= 5) {
            if (logger_) logger_->logAlertStillActive(**it);
            lastLoggedTime_[key] = now;
        }
        return;
    }

    // New alert
    auto alert = std::make_shared<Alert>(sev, msg, source);
    activeAlerts_.push_back(alert);
    alertHistory_.push_back(alert);
    stats_->recordAlert(sev);

    if (logger_) logger_->logAlertRaised(*alert);
    lastLoggedTime_[source + ":" + msg] = std::chrono::steady_clock::now();
}

void AlertManager::clearAlertBySource(const std::string& source) {
    for (auto& a : activeAlerts_) {
        if (a->getSource() == source) {
            if (logger_) logger_->logAlertCleared(a->getMessage(), source);
            lastLoggedTime_.erase(source + ":" + a->getMessage());
            a->deactivate();
        }
    }
    activeAlerts_.erase(
        std::remove_if(activeAlerts_.begin(), activeAlerts_.end(),
                       [](const std::shared_ptr<Alert>& a) { return !a->isActive(); }),
        activeAlerts_.end());
}

void AlertManager::evaluate(const VehicleData& data) {
    std::lock_guard<std::mutex> lock(alertMutex_);

    const double temp     = data.getEngineTemp();
    const double voltage  = data.getBatteryVoltage();
    const double speed    = data.getSpeed();
    const double pressure = data.getTirePressure();
    const bool   door     = data.isDoorOpen();
    const bool   belt     = data.isSeatbeltLocked();

    if (temp > Thresholds::ENGINE_TEMP_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "CRITICAL ENGINE OVERHEAT", "ENG_TEMP_01");
    else if (temp > Thresholds::ENGINE_TEMP_WARNING)
        raiseAlert(AlertSeverity::WARNING, "ENGINE TEMPERATURE HIGH", "ENG_TEMP_01");
    else
        clearAlertBySource("ENG_TEMP_01");

    if (voltage < Thresholds::BATTERY_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW BATTERY WARNING", "BAT_01");
    else if (voltage < Thresholds::BATTERY_WARNING)
        raiseAlert(AlertSeverity::WARNING, "BATTERY VOLTAGE LOW", "BAT_01");
    else
        clearAlertBySource("BAT_01");

    if (speed > Thresholds::SPEED_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "OVERSPEED WARNING", "SPD_01");
    else
        clearAlertBySource("SPD_01");

    if (pressure < Thresholds::TIRE_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW TIRE PRESSURE", "TIRE_01");
    else if (pressure < Thresholds::TIRE_WARNING)
        raiseAlert(AlertSeverity::WARNING, "TIRE PRESSURE WARNING", "TIRE_01");
    else
        clearAlertBySource("TIRE_01");

    if (door && speed > Thresholds::SPEED_DOOR_WARN)
        raiseAlert(AlertSeverity::WARNING, "DOOR OPEN WARNING", "DOOR_01");
    else
        clearAlertBySource("DOOR_01");

    if (!belt && speed > 0.0)
        raiseAlert(AlertSeverity::WARNING, "SEATBELT WARNING", "BELT_01");
    else
        clearAlertBySource("BELT_01");
}

std::vector<std::shared_ptr<Alert>> AlertManager::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return activeAlerts_;
}

std::vector<std::shared_ptr<Alert>> AlertManager::getAlertHistory() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return alertHistory_;
}

std::vector<std::shared_ptr<Alert>> AlertManager::filterActive(
    std::function<bool(const Alert&)> predicate) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;
    std::copy_if(activeAlerts_.begin(), activeAlerts_.end(),
                 std::back_inserter(result),
                 [&](const std::shared_ptr<Alert>& a) { return predicate(*a); });
    return result;
}

std::vector<std::shared_ptr<Alert>> AlertManager::searchHistory(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;
    std::copy_if(alertHistory_.begin(), alertHistory_.end(),
                 std::back_inserter(result),
                 [&](const std::shared_ptr<Alert>& a) {
                     return a->getMessage().find(keyword) != std::string::npos;
                 });
    return result;
}

std::vector<std::shared_ptr<Alert>> AlertManager::getBySeverity(AlertSeverity sev) const {
    return filterActive([sev](const Alert& a) { return a.getSeverity() == sev; });
}

int AlertManager::activeCount() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(activeAlerts_.size());
}

int AlertManager::historyCount() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(alertHistory_.size());
}