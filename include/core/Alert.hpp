// include/core/Alert.hpp
#pragma once
#include <string>
#include <chrono>

enum class AlertSeverity {
    INFO,
    WARNING,
    CRITICAL
};

class Alert {
private:
    static int alertCount;  // Static member
    
    int alertId;
    AlertSeverity severity;
    std::string message;
    std::string sensorSource;
    std::chrono::system_clock::time_point timestamp;
    bool isActive;
    
public:
    Alert(AlertSeverity sev, const std::string& msg, const std::string& source);
    
    // Copy and move semantics
    Alert(const Alert& other);
    Alert(Alert&& other) noexcept;
    Alert& operator=(const Alert& other);
    Alert& operator=(Alert&& other) noexcept;
    
    ~Alert();
    
    // Getters
    int getId() const;
    AlertSeverity getSeverity() const;
    std::string getMessage() const;
    std::string getTimestamp() const;
    
    // Operator overloading
    friend std::ostream& operator<<(std::ostream& os, const Alert& alert);
    bool operator<(const Alert& other) const;  // For set ordering
    
    // Static method
    static int getTotalAlerts();
    
    void deactivate();
};