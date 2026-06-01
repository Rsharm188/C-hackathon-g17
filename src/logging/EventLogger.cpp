#include "../../include/logging/EventLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <filesystem>

std::string EventLogger::timeNow() const {
    auto now    = std::chrono::system_clock::now();
    std::time_t t  = std::chrono::system_clock::to_time_t(now);
    std::tm*    tm = std::localtime(&t);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", tm);
    return std::string(buf);
}

EventLogger::EventLogger(const std::string& filePath)
    : filePath_(filePath)
{
    std::filesystem::create_directories(
        std::filesystem::path(filePath).parent_path()
    );
    logFile_.open(filePath_, std::ios::out | std::ios::app);
    if (!logFile_.is_open())
        throw LoggerException("Cannot open log file: " + filePath_);
    logSystemMessage("=== Vehicle Monitoring System Started ===");
}

EventLogger::~EventLogger() {
    try {
        logSystemMessage("=== Vehicle Monitoring System Stopped ===");
        if (logFile_.is_open()) {
            logFile_.flush();
            logFile_.close();
        }
    } catch (...) {}
}

void EventLogger::writeLine(const std::string& category, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!logFile_.is_open()) return;
    logFile_ << "[" << timeNow() << "] "
             << std::left << std::setw(16) << ("[" + category + "]")
             << " " << message << "\n";
    logFile_.flush();
}

void EventLogger::writeRaw(const std::string& text) {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!logFile_.is_open()) return;
    logFile_ << text;
    logFile_.flush();
}


void EventLogger::logEvent(const std::string& event) {
    writeLine("EVENT", event);
}

void EventLogger::logSystemMessage(const std::string& msg) {
    writeRaw("\n================================================================================\n");
    writeRaw(" " + msg + " — " + timeNow() + "\n");
    writeRaw("================================================================================\n\n");
}

void EventLogger::logAlertStillActive(const Alert& alert) {
    std::string sev;
    switch (alert.getSeverity()) {
        case AlertSeverity::CRITICAL: sev = "CRITICAL"; break;
        case AlertSeverity::WARNING:  sev = "WARNING";  break;
        case AlertSeverity::INFO:     sev = "INFO";     break;
    }
    writeLine("STILL ACTIVE",
              "— " + alert.getMessage() +
              " [" + sev + "]" +
              " | Source: " + alert.getSource() +
              " | ID: #"   + std::to_string(alert.getId()));
    writeRaw("\n");
}

void EventLogger::logSensorSnapshot(const VehicleData& data) {
    std::ostringstream oss;
     writeRaw("\n\n\n"); 
    oss << std::fixed << std::setprecision(1);
    oss << "Temp: "   << data.getEngineTemp()     << "C"
        << "  Volt: "  << data.getBatteryVoltage() << "V"
        << "  Speed: " << data.getSpeed()          << "km/h"
        << "  Tyre: "  << data.getTirePressure()   << "PSI"
        << "  Door: "  << (data.isDoorOpen()       ? "OPEN"     : "CLOSED")
        << "  Belt: "  << (data.isSeatbeltLocked() ? "LOCKED"   : "UNLOCKED");

    writeLine("SENSOR UPDATE", "— " + oss.str());
    writeRaw("\n");  // two blank lines after every sensor block
}

void EventLogger::logAlertRaised(const Alert& alert) {
    std::string sev;
    switch (alert.getSeverity()) {
        case AlertSeverity::CRITICAL: sev = "CRITICAL"; break;
        case AlertSeverity::WARNING:  sev = "WARNING";  break;
        case AlertSeverity::INFO:     sev = "INFO";     break;
    }
    writeLine("ALERT RAISED",
              "— " + alert.getMessage() +
              " [" + sev + "]" +
              " | Source: " + alert.getSource() +
              " | ID: #"   + std::to_string(alert.getId()));
    writeRaw("\n");
}

void EventLogger::logAlertCleared(const std::string& message, const std::string& source) {
    writeLine("ALERT CLEARED", "— " + message + " | Source: " + source);
    writeRaw("\n");
}

void EventLogger::flush() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) logFile_.flush();
}

std::string EventLogger::getFilePath() const { return filePath_; }

std::vector<std::string> EventLogger::searchLogs(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(logMutex_);
    std::vector<std::string> results;
    std::ifstream reader(filePath_);
    if (!reader.is_open())
        throw LoggerException("Cannot open log for search: " + filePath_);
    std::string line;
    while (std::getline(reader, line))
        if (line.find(keyword) != std::string::npos)
            results.push_back(line);
    return results;
}

std::vector<std::string> EventLogger::getCriticalEvents() const {
    return searchLogs("CRITICAL");
}