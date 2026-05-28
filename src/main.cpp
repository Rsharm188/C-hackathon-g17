#include <iostream>
#include <memory>
#include <csignal>
#include <atomic>
#include <chrono>
#include <thread>
#include <string>
#include <limits>

#include "../include/core/VehicleData.hpp"
#include "../include/managers/SensorManager.hpp"
#include "../include/managers/AlertManager.hpp"
#include "../include/managers/ThreadManager.hpp"
#include "../include/display/Dashboard.hpp"
#include "../include/logging/EventLogger.hpp"
#include "../include/utils/Statistics.hpp"
#include "../include/utils/ThreadSafeQueue.hpp"
#include "../include/utils/JsonParser.hpp"

using namespace std::chrono_literals;

static std::atomic<bool> g_shutdown{ false };
void signalHandler(int) { g_shutdown.store(true); }

static void runPostDemo(const std::shared_ptr<AlertManager>& alertMgr,
                        const std::shared_ptr<EventLogger>&  logger) {
    std::cout << "\n========================================\n";
    std::cout << " POST-RUN DEMONSTRATION\n";
    std::cout << "========================================\n";

    std::cout << "\n[Lambda Filter] CRITICAL alerts from history:\n";
    auto criticals = alertMgr->getBySeverity(AlertSeverity::CRITICAL);
    if (criticals.empty())
        std::cout << "  (none)\n";
    else
        for (const auto& a : criticals) std::cout << "  " << *a << "\n";

    std::cout << "\n[History Search] Alerts containing 'BATTERY':\n";
    auto batteryAlerts = alertMgr->searchHistory("BATTERY");
    if (batteryAlerts.empty())
        std::cout << "  (none)\n";
    else
        for (const auto& a : batteryAlerts) std::cout << "  " << *a << "\n";

    std::cout << "\n[Log Search] Lines containing 'CRITICAL':\n";
    try {
        auto lines = logger->getCriticalEvents();
        if (lines.empty()) {
            std::cout << "  (none)\n";
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

    std::cout << "\n[Static Members]\n"
              << "  Total Alert objects created  : " << Alert::getTotalAlerts()     << "\n"
              << "  Active Sensor instances      : " << Sensor::getTotalInstances() << "\n";
    std::cout << "========================================\n";
}

static void runJsonMode(const std::shared_ptr<VehicleData>&  vehicleData,
                        const std::shared_ptr<AlertManager>& alertMgr,
                        const std::shared_ptr<Dashboard>&    dashboard,
                        const std::shared_ptr<EventLogger>&  logger) {

    // List available test files
    std::vector<std::string> files;
    try {
        files = listTestFiles("tests");
    } catch (const std::exception& e) {
        std::cerr << "Could not read tests/ folder: " << e.what() << "\n";
        return;
    }

    if (files.empty()) {
        std::cerr << "No test files found in tests/\n";
        return;
    }
    while(1){
    std::cout << "\nAvailable Test Cases:\n";
    for (size_t i = 0; i < files.size(); ++i){
        std::cout << "  [" << i + 1 << "] " << files[i] << "\n";
    }
        std::cout <<"Or enter 'q' for exiting the code!\n";
    

    std::cout << "\nSelect test case: ";

        char choice1;
        std::cin >> choice1;
        if(choice1=='q'){
            break;
        }
        else{
            int choice = choice1-'0';
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice < 1 || choice > static_cast<int>(files.size())) {
            std::cerr << "Invalid choice.\n";
            return;
        }

        std::string selectedFile = "tests/" + files[choice - 1] + ".json";
        std::cout << "\nRunning: " << files[choice - 1] << "\n";
        std::cout << "Press Enter to step through each cycle...\n";

        std::vector<TestCase> cases;
        try {
            cases = loadTestCases(selectedFile);
        } catch (const std::exception& e) {
            std::cerr << "Failed to load: " << e.what() << "\n";
            return;
        }

        std::cout << "Loaded " << cases.size() << " test cases.\n";

        int i = 0;
        for (const auto& tc : cases) {
            std::cout << "\n[Press Enter next cycle " << ++i << "/" << cases.size() << "]";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            vehicleData->setEngineTemp(tc.engineTemp);
            vehicleData->setBatteryVoltage(tc.batteryVoltage);
            vehicleData->setSpeed(tc.speed);
            vehicleData->setTirePressure(tc.tirePressure);
            vehicleData->setDoorOpen(tc.doorOpen);
            vehicleData->setSeatbeltLocked(tc.seatbeltLocked);

            alertMgr->evaluate(*vehicleData);
            dashboard->render();

            logger->logEvent("TEST CASE " + std::to_string(i) +
                            " | Temp:"  + std::to_string(tc.engineTemp) +
                            " Volt:"   + std::to_string(tc.batteryVoltage) +
                            " Speed:"  + std::to_string(tc.speed));
        }

        std::cout << "\nAll cycles complete.\n";
        runPostDemo(alertMgr, logger);
        }
    }
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    std::cout << "=== Smart Cabin & Vehicle Health Monitoring System ===\n\n";
    std::cout << "Select Mode:\n";
    std::cout << "  [0] Live Simulation\n";
    std::cout << "  [1] JSON Test Mode\n";
    std::cout << "\nEnter choice: ";

    int mode = 0;
    std::cin >> mode;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (mode != 0 && mode != 1) {
        std::cerr << "Invalid choice. Defaulting to Mode 0.\n";
        mode = 0;
    }

    auto vehicleData = std::make_shared<VehicleData>();
    auto stats       = std::make_shared<VehicleStatistics>();
    auto sensorMgr   = std::make_shared<SensorManager>(vehicleData, stats);
    auto alertMgr    = std::make_shared<AlertManager>(stats);

    std::shared_ptr<EventLogger> logger;
    try {
        logger = std::make_shared<EventLogger>("logs/vehicle_log.txt");
    } catch (const LoggerException& e) {
        std::cerr << "FATAL: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    sensorMgr->initialise();
    auto dashboard = std::make_shared<Dashboard>(vehicleData, alertMgr, sensorMgr, stats);

    if (mode == 1) {
        runJsonMode(vehicleData, alertMgr, dashboard, logger);
        return EXIT_SUCCESS;
    }

    std::cout << "\n[Mode 0] Live Simulation starting. Press Ctrl+C to stop.\n\n";

    auto threadMgr = std::make_unique<ThreadManager>(
        sensorMgr, alertMgr, dashboard, logger, vehicleData);

    threadMgr->start();

    while (!g_shutdown.load())
        std::this_thread::sleep_for(200ms);

    std::cout << "\n[Main] Shutdown signal received — stopping threads...\n";
    threadMgr->stop();
    threadMgr->join();

    runPostDemo(alertMgr, logger);

    std::cout << "\n[Main] Clean shutdown complete.\n";
    return EXIT_SUCCESS;
}