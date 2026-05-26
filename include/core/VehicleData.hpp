// include/core/VehicleData.hpp
#pragma once
#include <mutex>
#include <map>
#include <string>

class VehicleData {
private:
    mutable std::mutex dataMutex;
    
    // Sensor readings
    double engineTemp;
    double batteryVoltage;
    double speed;
    double tirePressure;
    bool doorOpen;
    bool seatbeltLocked;
    
    std::map<std::string, double> additionalData;
    
public:
    VehicleData();
    
    // Thread-safe setters
    void setEngineTemp(double temp);
    void setBatteryVoltage(double voltage);
    void setSpeed(double spd);
    void setTirePressure(double pressure);
    void setDoorStatus(bool open);
    void setSeatbeltStatus(bool locked);
    
    // Thread-safe getters
    double getEngineTemp() const;
    double getBatteryVoltage() const;
    double getSpeed() const;
    double getTirePressure() const;
    bool isDoorOpen() const;
    bool isSeatbeltLocked() const;
    
    // Bulk data access
    std::map<std::string, double> getAllData() const;
};