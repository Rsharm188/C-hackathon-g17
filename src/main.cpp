#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>

#include "../include/core/VehicleData.hpp"
#include "../include/managers/SensorManager.hpp"
#include "../include/managers/AlertManager.hpp"
#include "../include/managers/ThreadManager.hpp"
#include "../include/display/Dashboard.hpp"
#include "../include/logging/EventLogger.hpp"
#include "../include/utils/Statistics.hpp"
#include "../include/utils/ThreadSafeQueue.hpp"

using namespace std::chrono_literals;

// ── Global shutdown flag (set by SIGINT handler) ───────────────────────────────
static std::atomic<bool> g_shutdown{ false };

void signalHandler(int /*sig*/) {
    g_shutdown.store(true);
}

// ── Demo: search and filter capabilities ──────────────────────────────────────
// Called after threads stop to demonstrate STL algorithm + lambda features
static void runPostDemo(const std::shared_ptr<AlertManager>&  alertMgr,
                        const std::shared_ptr<EventLogger>&   logger) {

    std::cout << "\n========================================\n";
    std::cout << " POST-RUN DEMONSTRATION\n";
    std::cout << "========================================\n";

    // 1. Filter alerts by severity using lambda
    std::cout << "\n[Lambda Filter] CRITICAL alerts from history:\n";
    auto criticals = alertMgr->getBySeverity(AlertSeverity::CRITICAL);
    if (criticals.empty()) {
        std::cout << "  (none — all clear during this run)\n";
    } else {
        for (const auto& a : criticals)
            std::cout << "  " << *a << "\n";
    }

    // 2. Search entire alert history for a keyword — demonstrates lambda + STL
    std::cout << "\n[History Search] Alerts containing 'BATTERY':\n";
    auto batteryAlerts = alertMgr->searchHistory("BATTERY");
    if (batteryAlerts.empty()) {
        std::cout << "  (none)\n";
    } else {
        for (const auto& a : batteryAlerts)
            std::cout << "  " << *a << "\n";
    }

    // 3. Search log file — demonstrates file streams + STL algorithms + lambda
    std::cout << "\n[Log File Search] Lines containing 'CRITICAL':\n";
    try {
        auto lines = logger->getCriticalEvents();
        if (lines.empty()) {
            std::cout << "  (none in log)\n";
        } else {
            int shown = 0;
            for (const auto& l : lines) {
                std::cout << "  " << l << "\n";
                if (++shown >= 5) { std::cout << "  ...and more\n"; break; }
            }
        }
    } catch (const LoggerException& e) {
        std::cerr << "Log search error: " << e.what() << "\n";
    }

    // 4. Static member check
    std::cout << "\n[Static Members]\n"
              << "  Total Alert objects ever created : " << Alert::getTotalAlerts()  << "\n"
              << "  Active Sensor instances          : " << Sensor::getTotalInstances() << "\n";

    std::cout << "\n========================================\n";
}

// ── main ───────────────────────────────────────────────────────────────────────
int main() {
    std::signal(SIGINT, signalHandler);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "=== Smart Cabin & Vehicle Health Monitoring System ===\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    // ── Build shared resources ─────────────────────────────────────────────────
    // Demonstrates: shared_ptr, unique_ptr, RAII

    auto vehicleData = std::make_shared<VehicleData>();
    auto stats       = std::make_shared<VehicleStatistics>();

    auto sensorMgr   = std::make_shared<SensorManager>(vehicleData, stats);
    auto alertMgr    = std::make_shared<AlertManager>(stats);

    // EventLogger constructor may throw — demonstrates Exception Handling
    std::shared_ptr<EventLogger> logger;
    try {
        logger = std::make_shared<EventLogger>("logs/vehicle_log.txt");
    } catch (const LoggerException& e) {
        std::cerr << "FATAL: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    sensorMgr->initialise();   // creates all 6 sensor objects (unique_ptr, polymorphism)

    auto dashboard = std::make_shared<Dashboard>(vehicleData, alertMgr, sensorMgr, stats);

    // ── Thread Manager owns and runs all threads ───────────────────────────────
    // Demonstrates: ThreadManager RAII — destructor joins all threads on exit
    auto threadMgr = std::make_unique<ThreadManager>(
        sensorMgr, alertMgr, dashboard, logger, vehicleData);

    threadMgr->start();

    // ── Run until SIGINT ───────────────────────────────────────────────────────
    while (!g_shutdown.load()) {
        std::this_thread::sleep_for(200ms);
    }

    std::cout << "\n[Main] Shutdown signal received — stopping threads...\n";
    threadMgr->stop();
    threadMgr->join();

    // ── Post-run demo of search / filter / STL features ───────────────────────
    runPostDemo(alertMgr, logger);

    std::cout << "\n[Main] Clean shutdown complete. No memory leaks (smart pointers).\n";
    return EXIT_SUCCESS;
}