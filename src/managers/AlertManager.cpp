#include "../../include/managers/AlertManager.hpp"
#include "../../include/core/Types.hpp"
#include <algorithm>
#include <iostream>

// Constructor — takes shared ownership of both Statistics and EventLogger.
// Using std::move avoids a ref-count bump; the caller's shared_ptrs become null.
AlertManager::AlertManager(std::shared_ptr<VehicleStatistics> stats,
                           std::shared_ptr<EventLogger>       logger)
    : stats_(std::move(stats)), logger_(std::move(logger))
{}

// ── raiseAlert ────────────────────────────────────────────────────────────────
// Private helper — only called from evaluate(), which already holds alertMutex_.
// Do NOT call this from any public method or a deadlock will occur (std::mutex
// is non-recursive).
void AlertManager::raiseAlert(AlertSeverity sev,
                               const std::string& msg,
                               const std::string& source) {

    // Lambda captures source and msg by reference to check for an existing
    // active alert with the same source+message pair before creating a new one.
    auto isDuplicate = [&](const std::shared_ptr<Alert>& a) {
        return a->isActive() && a->getSource() == source && a->getMessage() == msg;
    };

    auto it = std::find_if(activeAlerts_.begin(), activeAlerts_.end(), isDuplicate);
    if (it != activeAlerts_.end()) {
        // Alert is already active — don't create a duplicate.
        // Instead, re-log it every 5 seconds as a reminder so the log file
        // reflects that the condition is still ongoing, not just when it started.
        auto now = std::chrono::steady_clock::now();

        // Key is "source:message" — uniquely identifies this alert condition.
        auto key    = source + ":" + msg;
        auto lastIt = lastLoggedTime_.find(key);

        // Log if this key has never been logged, or if 5 seconds have elapsed
        // since the last reminder log entry.
        if (lastIt == lastLoggedTime_.end() ||
            std::chrono::duration_cast<std::chrono::seconds>(
                now - lastIt->second).count() >= 5) {

            // Writes a [STILL ACTIVE] line to the log file with severity + ID.
            if (logger_) logger_->logAlertStillActive(**it);
            lastLoggedTime_[key] = now;
        }
        return;
    }

    // New alert — construct it on the heap and share ownership between
    // activeAlerts_ (current state) and alertHistory_ (permanent record).
    auto alert = std::make_shared<Alert>(sev, msg, source);
    activeAlerts_.push_back(alert);
    alertHistory_.push_back(alert);   // history is never erased

    // Update severity counters (totalAlerts, criticalAlerts, warningAlerts, etc.)
    stats_->recordAlert(sev);

    // Write an [ALERT RAISED] line to the log file.
    if (logger_) logger_->logAlertRaised(*alert);

    // Seed the reminder timer so the 5-second window starts from first raise.
    lastLoggedTime_[source + ":" + msg] = std::chrono::steady_clock::now();
}

// ── clearAlertBySource ────────────────────────────────────────────────────────
// Private helper — only called from evaluate(), which already holds alertMutex_.
// Deactivates all active alerts from a given source (e.g. "ENG_TEMP_01") and
// removes them from activeAlerts_. alertHistory_ is intentionally untouched.
void AlertManager::clearAlertBySource(const std::string& source) {
    for (auto& a : activeAlerts_) {
        if (a->getSource() == source) {
            // Write an [ALERT CLEARED] line before deactivating.
            if (logger_) logger_->logAlertCleared(a->getMessage(), source);

            // Remove the reminder timer entry so if the condition re-triggers
            // later, it is treated as a fresh alert, not a continuation.
            lastLoggedTime_.erase(source + ":" + a->getMessage());

            a->deactivate();
        }
    }

    // Erase-remove idiom: remove_if shifts inactive alerts to the end,
    // then erase trims the vector in a single O(n) pass.
    activeAlerts_.erase(
        std::remove_if(activeAlerts_.begin(), activeAlerts_.end(),
                       [](const std::shared_ptr<Alert>& a) { return !a->isActive(); }),
        activeAlerts_.end());
}

// ── evaluate ──────────────────────────────────────────────────────────────────
// Called by the Monitor Thread every 750 ms. Acquires alertMutex_ for the
// entire evaluation so no other thread can read a half-updated alert list.
// raiseAlert and clearAlertBySource are private and only called from here,
// so they intentionally do NOT acquire the mutex themselves — doing so would
// deadlock since std::mutex is non-recursive.
void AlertManager::evaluate(const VehicleData& data) {
    std::lock_guard<std::mutex> lock(alertMutex_);

    // Read a consistent snapshot. VehicleData's own per-field mutexes ensure
    // each getter returns a coherent value even while the Sensor Thread writes.
    const double temp     = data.getEngineTemp();
    const double voltage  = data.getBatteryVoltage();
    const double speed    = data.getSpeed();
    const double pressure = data.getTirePressure();
    const bool   door     = data.isDoorOpen();
    const bool   belt     = data.isSeatbeltLocked();

    // ── Engine Temperature ────────────────────────────────────────────────────
    // Two-level threshold: WARNING at 100°C, CRITICAL at 115°C (from Types.hpp).
    // Clearing by source ensures a WARNING is removed if temp drops below 100°C,
    // even if a CRITICAL was previously active for the same source.
    if (temp > Thresholds::ENGINE_TEMP_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "CRITICAL ENGINE OVERHEAT", "ENG_TEMP_01");
    else if (temp > Thresholds::ENGINE_TEMP_WARNING)
        raiseAlert(AlertSeverity::WARNING, "ENGINE TEMPERATURE HIGH", "ENG_TEMP_01");
    else
        clearAlertBySource("ENG_TEMP_01");

    // ── Battery Voltage ───────────────────────────────────────────────────────
    // Two-level threshold: WARNING at 11.5V, CRITICAL at 11.0V.
    if (voltage < Thresholds::BATTERY_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW BATTERY WARNING", "BAT_01");
    else if (voltage < Thresholds::BATTERY_WARNING)
        raiseAlert(AlertSeverity::WARNING, "BATTERY VOLTAGE LOW", "BAT_01");
    else
        clearAlertBySource("BAT_01");

    // ── Speed ─────────────────────────────────────────────────────────────────
    // Single threshold: CRITICAL above 150 km/h.
    if (speed > Thresholds::SPEED_CRITICAL)
        raiseAlert(AlertSeverity::CRITICAL, "OVERSPEED WARNING", "SPD_01");
    else
        clearAlertBySource("SPD_01");

    // ── Tire Pressure ─────────────────────────────────────────────────────────
    // Two-level threshold: WARNING at 25 PSI, CRITICAL at 20 PSI.
    if (pressure < Thresholds::TIRE_LOW)
        raiseAlert(AlertSeverity::CRITICAL, "LOW TIRE PRESSURE", "TIRE_01");
    else if (pressure < Thresholds::TIRE_WARNING)
        raiseAlert(AlertSeverity::WARNING, "TIRE PRESSURE WARNING", "TIRE_01");
    else
        clearAlertBySource("TIRE_01");

    // ── Door while moving ─────────────────────────────────────────────────────
    // Only warns if speed exceeds SPEED_DOOR_WARN (5 km/h) — avoids a false
    // alert when the door is opened while stationary (e.g. parking).
    if (door && speed > Thresholds::SPEED_DOOR_WARN)
        raiseAlert(AlertSeverity::WARNING, "DOOR OPEN WARNING", "DOOR_01");
    else
        clearAlertBySource("DOOR_01");

    // ── Seatbelt while moving ─────────────────────────────────────────────────
    // Any non-zero speed triggers this — belt must be locked whenever the
    // vehicle is in motion, even at very low speeds.
    if (!belt && speed > 0.0)
        raiseAlert(AlertSeverity::WARNING, "SEATBELT WARNING", "BELT_01");
    else
        clearAlertBySource("BELT_01");
}

// ── Thread-safe accessors ─────────────────────────────────────────────────────
// Each returns a copy of the internal vector — the caller gets a snapshot that
// remains valid even if evaluate() modifies the original on another thread.

std::vector<std::shared_ptr<Alert>> AlertManager::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return activeAlerts_;
}

std::vector<std::shared_ptr<Alert>> AlertManager::getAlertHistory() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return alertHistory_;
}

// filterActive — generic predicate-based filter over active alerts.
// Takes a std::function so callers can pass lambdas, function pointers,
// or functors without knowing the internal container type.
std::vector<std::shared_ptr<Alert>> AlertManager::filterActive(
    std::function<bool(const Alert&)> predicate) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;

    // std::copy_if + std::back_inserter: only copies alerts where the
    // predicate returns true, appending to result without pre-sizing it.
    std::copy_if(activeAlerts_.begin(), activeAlerts_.end(),
                 std::back_inserter(result),
                 [&](const std::shared_ptr<Alert>& a) { return predicate(*a); });
    return result;
}

// searchHistory — scans the all-time alert history for alerts whose message
// contains the given keyword (case-sensitive substring match).
std::vector<std::shared_ptr<Alert>> AlertManager::searchHistory(
    const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    std::vector<std::shared_ptr<Alert>> result;
    std::copy_if(alertHistory_.begin(), alertHistory_.end(),
                 std::back_inserter(result),
                 [&](const std::shared_ptr<Alert>& a) {
                     return a->getMessage().find(keyword) != std::string::npos;
                 });
    return result;
}

// getBySeverity — convenience wrapper around filterActive.
// The lambda captures sev by value (safe: AlertSeverity is a trivial enum).
std::vector<std::shared_ptr<Alert>> AlertManager::getBySeverity(
    AlertSeverity sev) const {
    return filterActive([sev](const Alert& a) { return a.getSeverity() == sev; });
}

// Note: getBySeverity calls filterActive, which acquires alertMutex_.
// This is safe because getBySeverity itself does NOT hold the lock —
// it delegates entirely to filterActive. No nested locking occurs.

int AlertManager::activeCount() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(activeAlerts_.size());
}

int AlertManager::historyCount() const {
    std::lock_guard<std::mutex> lock(alertMutex_);
    return static_cast<int>(alertHistory_.size());
}
