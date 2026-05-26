// include/logging/EventLogger.hpp
#pragma once
#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <queue>
#include "Alert.hpp"

class EventLogger {
private:
    std::ofstream logFile;  // RAII: file handle
    std::string logFilePath;
    std::mutex logMutex;
    std::queue<std::string> logQueue;
    
    bool isOpen;
    
    std::string formatLogEntry(const std::string& message);
    
public:
    EventLogger(const std::string& filePath);
    ~EventLogger();  // RAII cleanup
    
    // Prevent copying
    EventLogger(const EventLogger&) = delete;
    EventLogger& operator=(const EventLogger&) = delete;
    
    void logAlert(const Alert& alert);
    void logEvent(const std::string& event);
    void logSensorData(const std::string& sensorId, double value);
    
    void flush();
    
    // Search functionality using STL algorithms
    std::vector<std::string> searchLogs(const std::string& keyword);
};