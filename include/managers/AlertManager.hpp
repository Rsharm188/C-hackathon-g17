// include/managers/AlertManager.hpp
#pragma once
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include "core/Alert.hpp"
#include "core/VehicleData.hpp"
#include "utils/Statistics.hpp"
#include "logging/EventLogger.hpp"
#include <chrono>
#include <map>


class AlertManager {
private:
    std::vector<std::shared_ptr<Alert>> activeAlerts_;
    std::vector<std::shared_ptr<Alert>> alertHistory_;
    mutable std::mutex                  alertMutex_;
    std::shared_ptr<VehicleStatistics>  stats_;
    std::shared_ptr<EventLogger>        logger_;
   
    std::map<std::string, std::chrono::steady_clock::time_point> lastLoggedTime_;

    void raiseAlert(AlertSeverity sev, const std::string& msg, const std::string& source);
    void clearAlertBySource(const std::string& source);

public:
    AlertManager(std::shared_ptr<VehicleStatistics> stats,
                 std::shared_ptr<EventLogger>       logger);

    void evaluate(const VehicleData& data);

    

    std::vector<std::shared_ptr<Alert>> getActiveAlerts()  const;
    std::vector<std::shared_ptr<Alert>> getAlertHistory()  const;
    std::vector<std::shared_ptr<Alert>> filterActive(std::function<bool(const Alert&)> predicate) const;
    std::vector<std::shared_ptr<Alert>> searchHistory(const std::string& keyword) const;
    std::vector<std::shared_ptr<Alert>> getBySeverity(AlertSeverity sev) const;

    int activeCount()  const;
    int historyCount() const;
};