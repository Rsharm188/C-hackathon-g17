// include/managers/ThreadManager.hpp
#pragma once
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include "SensorManager.hpp"
#include "AlertManager.hpp"
#include "Dashboard.hpp"
#include "EventLogger.hpp"

class ThreadManager {
private:
    std::vector<std::thread> threads;
    std::atomic<bool> running;
    
    std::shared_ptr<SensorManager> sensorManager;
    std::shared_ptr<AlertManager> alertManager;
    std::shared_ptr<Dashboard> dashboard;
    std::shared_ptr<EventLogger> logger;
    std::shared_ptr<VehicleData> vehicleData;
    
    // Thread functions
    void sensorUpdateThread();
    void monitoringThread();
    void dashboardThread();
    void loggerThread();
    
public:
    ThreadManager(std::shared_ptr<SensorManager> sm,
                  std::shared_ptr<AlertManager> am,
                  std::shared_ptr<Dashboard> db,
                  std::shared_ptr<EventLogger> log,
                  std::shared_ptr<VehicleData> vd);
    
    ~ThreadManager();
    
    void start();
    void stop();
    void join();
};