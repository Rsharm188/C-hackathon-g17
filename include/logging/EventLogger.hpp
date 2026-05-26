// include/logging/EventLogger.hpp
#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <stdexcept>
#include "core/Alert.hpp"

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

public:
    explicit EventLogger(const std::string& filePath);
    ~EventLogger();

    EventLogger(const EventLogger&) = delete;
    EventLogger& operator=(const EventLogger&) = delete;

    void logAlert(const Alert& alert);
    void logEvent(const std::string& event);
    void logSensorReading(const std::string& sensorId, double value, const std::string& unit);
    void logSystemMessage(const std::string& msg);

    std::vector<std::string> searchLogs(const std::string& keyword) const;
    std::vector<std::string> getCriticalEvents() const;

    void        flush();
    std::string getFilePath() const;
};