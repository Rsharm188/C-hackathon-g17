#include "../../include/logging/EventLogger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

// ── Constructor (RAII — opens file) ───────────────────────────────────────────
// Demonstrates: RAII, Exception Handling (throws on failure)
EventLogger::EventLogger(const std::string& filePath)
    : filePath_(filePath)
{
    // Throws LoggerException if file cannot be opened — exception-safe design
    try {
        logFile_.open(filePath_, std::ios::out | std::ios::app);
        if (!logFile_.is_open())
            throw LoggerException("Cannot open log file: " + filePath_);
    } catch (const std::exception& e) {
        throw LoggerException(e.what());
    }

    logSystemMessage("=== Vehicle Monitoring System Started ===");
}

// ── Destructor (RAII — closes file) ───────────────────────────────────────────
EventLogger::~EventLogger() {
    try {
        logSystemMessage("=== Vehicle Monitoring System Stopped ===");
        if (logFile_.is_open()) {
            logFile_.flush();
            logFile_.close();
        }
    } catch (...) {
        // Never throw from destructor
    }
}

// ── Internal writeLine ─────────────────────────────────────────────────────────
void EventLogger::writeLine(const std::string& category, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (!logFile_.is_open()) return;

    // Demonstrates: file streams (std::ofstream)
    logFile_ << "[" << currentTimestamp() << "] "
             << "[" << category << "] "
             << message << "\n";
    logFile_.flush();
}

// ── Public logging methods ─────────────────────────────────────────────────────
void EventLogger::logAlert(const Alert& alert) {
    std::ostringstream oss;
    oss << alert;   // uses operator<< on Alert — operator overloading in action
    writeLine(severityToString(alert.getSeverity()), oss.str());
}

void EventLogger::logEvent(const std::string& event) {
    writeLine("EVENT", event);
}

void EventLogger::logSensorReading(const std::string& sensorId,
                                    double value,
                                    const std::string& unit) {
    writeLine("SENSOR", sensorId + " = " + std::to_string(value) + " " + unit);
}

void EventLogger::logSystemMessage(const std::string& msg) {
    writeLine("SYSTEM", msg);
}

// ── searchLogs — demonstrates: file streams + STL algorithms + lambdas ─────────
std::vector<std::string> EventLogger::searchLogs(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(logMutex_);
    std::vector<std::string> results;

    // Exception handling for file read failure
    try {
        std::ifstream reader(filePath_);
        if (!reader.is_open())
            throw LoggerException("Cannot open log for search: " + filePath_);

        std::string line;
        while (std::getline(reader, line)) {
            // Lambda: true if line contains keyword (used with copy_if pattern)
            auto containsKeyword = [&keyword](const std::string& l) {
                return l.find(keyword) != std::string::npos;
            };
            if (containsKeyword(line))
                results.push_back(line);
        }
    } catch (const LoggerException&) {
        throw;
    } catch (const std::exception& e) {
        throw LoggerException(std::string("Search failed: ") + e.what());
    }

    return results;
}

// ── getCriticalEvents — lambda filtering over file lines ──────────────────────
std::vector<std::string> EventLogger::getCriticalEvents() const {
    return searchLogs("[CRITICAL]");
}

void EventLogger::flush() {
    std::lock_guard<std::mutex> lock(logMutex_);
    if (logFile_.is_open()) logFile_.flush();
}

std::string EventLogger::getFilePath() const { return filePath_; }