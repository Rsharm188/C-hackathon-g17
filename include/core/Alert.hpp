// include/core/Alert.hpp
#pragma once
#include <string>
#include <chrono>
#include <ostream>

enum class AlertSeverity { INFO, WARNING, CRITICAL };

inline std::string severityToString(AlertSeverity s) {
    switch (s) {
        case AlertSeverity::CRITICAL: return "CRITICAL";
        case AlertSeverity::WARNING:  return "WARNING";
        case AlertSeverity::INFO:     return "INFO";
    }
    return "UNKNOWN";
}

inline std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::string s = std::ctime(&t);
    if (!s.empty() && s.back() == '\n') s.pop_back();
    return s;
}

class Alert {
private:
    static int totalAlertCount;

    int alertId;
    AlertSeverity severity;
    std::string message;
    std::string sensorSource;
    std::string timestamp;
    bool active;

public:
    Alert(AlertSeverity sev, const std::string& msg, const std::string& source);

    Alert(const Alert& other);
    Alert(Alert&& other) noexcept;
    Alert& operator=(const Alert& other);
    Alert& operator=(Alert&& other) noexcept;
    ~Alert();

    int           getId()         const;
    AlertSeverity getSeverity()   const;
    std::string   getMessage()    const;
    std::string   getSource()     const;
    std::string   getTimestamp()  const;
    bool          isActive()      const;
    void          deactivate();

    static int getTotalAlerts();

    friend std::ostream& operator<<(std::ostream& os, const Alert& alert);
    bool operator<(const Alert& other) const;
    bool operator==(const Alert& other) const;
};