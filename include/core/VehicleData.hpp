// include/core/VehicleData.hpp
#pragma once
#include <mutex>
#include <map>
#include <string>

class VehicleData {
private:
    mutable std::mutex dataMutex;
    double engineTemp     = 85.0;
    double batteryVoltage = 12.5;
    double speed          = 0.0;
    double tirePressure   = 30.0;
    bool   doorOpen       = false;
    bool   seatbeltLocked = true;

public:
    VehicleData() = default;

    void setEngineTemp(double val);
    void setBatteryVoltage(double val);
    void setSpeed(double val);
    void setTirePressure(double val);
    void setDoorOpen(bool val);
    void setSeatbeltLocked(bool val);

    double getEngineTemp()     const;
    double getBatteryVoltage() const;
    double getSpeed()          const;
    double getTirePressure()   const;
    bool   isDoorOpen()        const;
    bool   isSeatbeltLocked()  const;

    std::map<std::string, double> snapshot() const;
};