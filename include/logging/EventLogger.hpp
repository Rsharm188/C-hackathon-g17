// include/logging/EventLogger.hpp
#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <stdexcept>
#include "core/Alert.hpp"
#include "core/VehicleData.hpp"

class LoggerException : public std::runtime_error {
public:
    explicit LoggerException(const std::string& msg) : std::runtime_error(msg) {}
};

class EventLogger {
private:
    std::string   filePath_;
    std::ofstream logFile_;
    mutable std::mutex logMutex_;

    void writeLine(const std::string& category, const std::string& message);
    void writeRaw(const std::string& text);
    std::string timeNow() const;

public:
    explicit EventLogger(const std::string& filePath);
    ~EventLogger();

    EventLogger(const EventLogger&) = delete;
    EventLogger& operator=(const EventLogger&) = delete;

    void logAlertStillActive(const Alert& alert);

    // Old methods (kept for compatibility)
    void logEvent(const std::string& event);
    void logSystemMessage(const std::string& msg);

    // New structured methods
    void logSensorSnapshot(const VehicleData& data);
    void logAlertRaised(const Alert& alert);
    void logAlertCleared(const std::string& message, const std::string& source);

    std::vector<std::string> searchLogs(const std::string& keyword) const;
    std::vector<std::string> getCriticalEvents() const;

    void        flush();
    std::string getFilePath() const;
};