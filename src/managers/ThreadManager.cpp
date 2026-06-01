#include "../../include/managers/ThreadManager.hpp"
#include <iostream>
#include <chrono>
#include <sstream>

using namespace std::chrono_literals;

// ── Constructor ────────────────────────────────────────────────────────────────
ThreadManager::ThreadManager(std::shared_ptr<SensorManager>  sm,
                              std::shared_ptr<AlertManager>   am,
                              std::shared_ptr<Dashboard>      db,
                              std::shared_ptr<EventLogger>    log,
                              std::shared_ptr<VehicleData>    vd)
    : sensorManager_(std::move(sm)),
      alertManager_(std::move(am)),
      dashboard_(std::move(db)),
      logger_(std::move(log)),
      vehicleData_(std::move(vd))
{}

// ── Destructor (RAII — ensures clean thread shutdown) ─────────────────────────
ThreadManager::~ThreadManager() {
    stop();
    join();
}

// ── start ──────────────────────────────────────────────────────────────────────
// Demonstrates: std::thread creation, atomic flag
void ThreadManager::start() {
    running_.store(true);

    // Launch four concurrent threads — each binds a member function
    sensorThread_   = std::thread(&ThreadManager::runSensorThread,    this);
    monitorThread_  = std::thread(&ThreadManager::runMonitorThread,   this);
    dashboardThread_= std::thread(&ThreadManager::runDashboardThread, this);
    loggerThread_   = std::thread(&ThreadManager::runLoggerThread,    this);

    std::cout << "[ThreadManager] All threads started.\n";
}

// ── stop ───────────────────────────────────────────────────────────────────────
void ThreadManager::stop() {
    running_.store(false);
    logQueue_.close();   // wake logger thread if it's blocked
}

// ── join ───────────────────────────────────────────────────────────────────────
// Demonstrates: thread joining, RAII lifetime management
void ThreadManager::join() {
    if (sensorThread_.joinable())    sensorThread_.join();
    if (monitorThread_.joinable())   monitorThread_.join();
    if (dashboardThread_.joinable()) dashboardThread_.join();
    if (loggerThread_.joinable())    loggerThread_.join();
}

bool ThreadManager::isRunning() const { return running_.load(); }

// ── pushLogEvent ───────────────────────────────────────────────────────────────
// Any thread can push to the shared log queue safely (ThreadSafeQueue uses mutex)
void ThreadManager::pushLogEvent(const std::string& event) {
    logQueue_.push(event);
}

// ════════════════════════════════════════════════════════════════════════════════
//  Thread Functions
// ════════════════════════════════════════════════════════════════════════════════

// ── Sensor Thread ──────────────────────────────────────────────────────────────
// Responsibility: Periodically updates all sensors and writes to VehicleData
// Demonstrates: std::thread, mutex (inside SensorManager & VehicleData),
//               runtime polymorphism via sensorManager_->updateAll()
void ThreadManager::runSensorThread() {
    while (running_.load()) {
        sensorManager_->updateAll();
        logger_->logSensorSnapshot(*vehicleData_);
        std::this_thread::sleep_for(500ms);
    }
}

// ── Monitoring Thread ──────────────────────────────────────────────────────────
// Responsibility: Reads VehicleData, evaluates alert conditions
// Demonstrates: mutex (inside VehicleData & AlertManager), shared_ptr
void ThreadManager::runMonitorThread() {
    while (running_.load()) {
        alertManager_->evaluate(*vehicleData_);
        std::this_thread::sleep_for(750ms);
    }
}

// ── Dashboard Thread ───────────────────────────────────────────────────────────
// Responsibility: Refreshes the console display and exports a file snapshot
// Demonstrates: stream operators via dashboard_->render(), file export
void ThreadManager::runDashboardThread() {
    int cycleCount = 0;
    while (running_.load()) {
        dashboard_->render();
        std::cout << *dashboard_ << "\n";   // stream operator<<

        // Export snapshot to file every 10 cycles
        if (++cycleCount % 10 == 0)
            dashboard_->exportSnapshot("logs/snapshots.txt");

        std::this_thread::sleep_for(1000ms);   // 1 Hz display rate
    }
}

// ── Logger Thread ──────────────────────────────────────────────────────────────
// Responsibility: Drains the shared log queue and writes entries to the log file
// Demonstrates: ThreadSafeQueue (template), mutex, RAII file handle in EventLogger
void ThreadManager::runLoggerThread() {
    while (running_.load() || !logQueue_.empty()) {
        // Blocking wait up to 500ms for a log entry
        auto entry = logQueue_.waitPop(500);
        if (entry.has_value()) {
            logger_->logEvent(entry.value());
        }
    }
}