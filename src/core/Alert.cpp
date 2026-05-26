#include "../../include/core/Alert.hpp"
#include <iostream>
#include <iomanip>
#include <utility>

// ── Static member definition ───────────────────────────────────────────────────
int Alert::totalAlertCount = 0;

// ── Parameterised constructor ──────────────────────────────────────────────────
Alert::Alert(AlertSeverity sev, const std::string& msg, const std::string& source)
    : alertId(++totalAlertCount),
      severity(sev),
      message(msg),
      sensorSource(source),
      timestamp(currentTimestamp()),
      active(true)
{}

// ── Copy constructor ───────────────────────────────────────────────────────────
// Demonstrates: Copy Semantics — a full deep copy (all members are value types)
Alert::Alert(const Alert& other)
    : alertId(other.alertId),
      severity(other.severity),
      message(other.message),
      sensorSource(other.sensorSource),
      timestamp(other.timestamp),
      active(other.active)
{}

// ── Move constructor ───────────────────────────────────────────────────────────
// Demonstrates: Move Semantics — transfers string resources without copying
Alert::Alert(Alert&& other) noexcept
    : alertId(other.alertId),
      severity(other.severity),
      message(std::move(other.message)),
      sensorSource(std::move(other.sensorSource)),
      timestamp(std::move(other.timestamp)),
      active(other.active)
{
    other.active  = false;
    other.alertId = -1;
}

// ── Copy assignment ────────────────────────────────────────────────────────────
Alert& Alert::operator=(const Alert& other) {
    if (this != &other) {
        alertId      = other.alertId;
        severity     = other.severity;
        message      = other.message;
        sensorSource = other.sensorSource;
        timestamp    = other.timestamp;
        active       = other.active;
    }
    return *this;
}

// ── Move assignment ────────────────────────────────────────────────────────────
Alert& Alert::operator=(Alert&& other) noexcept {
    if (this != &other) {
        alertId      = other.alertId;
        severity     = other.severity;
        message      = std::move(other.message);
        sensorSource = std::move(other.sensorSource);
        timestamp    = std::move(other.timestamp);
        active       = other.active;
        other.active  = false;
        other.alertId = -1;
    }
    return *this;
}

// ── Destructor ─────────────────────────────────────────────────────────────────
Alert::~Alert() = default;

// ── Getters ────────────────────────────────────────────────────────────────────
int           Alert::getId()        const { return alertId; }
AlertSeverity Alert::getSeverity()  const { return severity; }
std::string   Alert::getMessage()   const { return message; }
std::string   Alert::getSource()    const { return sensorSource; }
std::string   Alert::getTimestamp() const { return timestamp; }
bool          Alert::isActive()     const { return active; }
void          Alert::deactivate()         { active = false; }
int           Alert::getTotalAlerts()     { return totalAlertCount; }

// ── Operator << ────────────────────────────────────────────────────────────────
// Demonstrates: Operator Overloading — formatted alert printing as required
std::ostream& operator<<(std::ostream& os, const Alert& a) {
    os << "[" << std::setw(8) << severityToString(a.severity) << "] "
       << std::setw(35) << a.message
       << " | Source: " << std::setw(25) << a.sensorSource
       << " | Time: "   << a.timestamp
       << " | ID: #"    << a.alertId;
    return os;
}

// ── Operator < (for set ordering) ─────────────────────────────────────────────
bool Alert::operator<(const Alert& other) const {
    // Higher severity first; within same severity, lower id first
    if (severity != other.severity)
        return static_cast<int>(severity) > static_cast<int>(other.severity);
    return alertId < other.alertId;
}

bool Alert::operator==(const Alert& other) const {
    return alertId == other.alertId;
}