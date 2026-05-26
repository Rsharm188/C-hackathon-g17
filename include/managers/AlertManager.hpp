// include/managers/AlertManager.hpp
#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include "core/Alert.hpp"         // not "Alert.hpp"
#include "core/VehicleData.hpp"   // not "VehicleData.hpp"
#include "utils/Statistics.hpp"

class AlertManager {
private:
    std::vector<std::shared_ptr<Alert>> activeAlerts_;
    std::vector<std::shared_ptr<Alert>> alertHistory_;
    mutable std::mutex alertMutex_;
    std::shared_ptr<VehicleStatistics> stats_;

    void raiseAlert(AlertSeverity sev, const std::string& msg, const std::string& source);
    void clearAlertBySource(const std::string& source);

public:
    explicit AlertManager(std::shared_ptr<VehicleStatistics> stats);

    void evaluate(const VehicleData& data);

    std::vector<std::shared_ptr<Alert>> getActiveAlerts()  const;
    std::vector<std::shared_ptr<Alert>> getAlertHistory()  const;
    std::vector<std::shared_ptr<Alert>> filterActive(std::function<bool(const Alert&)> predicate) const;
    std::vector<std::shared_ptr<Alert>> searchHistory(const std::string& keyword) const;
    std::vector<std::shared_ptr<Alert>> getBySeverity(AlertSeverity sev) const;

    int activeCount()  const;
    int historyCount() const;
};