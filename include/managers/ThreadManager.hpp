// include/managers/ThreadManager.hpp
#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include "managers/SensorManager.hpp"
#include "managers/AlertManager.hpp"
#include "display/Dashboard.hpp"
#include "logging/EventLogger.hpp"
#include "utils/ThreadSafeQueue.hpp"

class ThreadManager {
private:
    std::atomic<bool> running_{ false };

    std::shared_ptr<SensorManager>  sensorManager_;
    std::shared_ptr<AlertManager>   alertManager_;
    std::shared_ptr<Dashboard>      dashboard_;
    std::shared_ptr<EventLogger>    logger_;
    std::shared_ptr<VehicleData>    vehicleData_;

    ThreadSafeQueue<std::string> logQueue_;

    std::thread sensorThread_;
    std::thread monitorThread_;
    std::thread dashboardThread_;
    std::thread loggerThread_;

    void runSensorThread();
    void runMonitorThread();
    void runDashboardThread();
    void runLoggerThread();

    void pushLogEvent(const std::string& event);

public:
    ThreadManager(std::shared_ptr<SensorManager>  sm,
                  std::shared_ptr<AlertManager>   am,
                  std::shared_ptr<Dashboard>      db,
                  std::shared_ptr<EventLogger>    log,
                  std::shared_ptr<VehicleData>    vd);
    ~ThreadManager();

    void start();
    void stop();
    void join();
    bool isRunning() const;
};