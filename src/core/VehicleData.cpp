#include "../../include/core/VehicleData.hpp"

// ── Thread-safe setters ────────────────────────────────────────────────────────
// Each uses lock_guard for RAII-style mutex ownership (no manual unlock needed)
void VehicleData::setEngineTemp(double val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    engineTemp = val;
}
void VehicleData::setBatteryVoltage(double val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    batteryVoltage = val;
}
void VehicleData::setSpeed(double val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    speed = val;
}
void VehicleData::setTirePressure(double val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    tirePressure = val;
}
void VehicleData::setDoorOpen(bool val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    doorOpen = val;
}
void VehicleData::setSeatbeltLocked(bool val) {
    std::lock_guard<std::mutex> lock(dataMutex);
    seatbeltLocked = val;
}

// ── Thread-safe getters ────────────────────────────────────────────────────────
double VehicleData::getEngineTemp()     const {
    std::lock_guard<std::mutex> lock(dataMutex); return engineTemp;
}
double VehicleData::getBatteryVoltage() const {
    std::lock_guard<std::mutex> lock(dataMutex); return batteryVoltage;
}
double VehicleData::getSpeed()          const {
    std::lock_guard<std::mutex> lock(dataMutex); return speed;
}
double VehicleData::getTirePressure()   const {
    std::lock_guard<std::mutex> lock(dataMutex); return tirePressure;
}
bool VehicleData::isDoorOpen()          const {
    std::lock_guard<std::mutex> lock(dataMutex); return doorOpen;
}
bool VehicleData::isSeatbeltLocked()    const {
    std::lock_guard<std::mutex> lock(dataMutex); return seatbeltLocked;
}

// ── Snapshot ───────────────────────────────────────────────────────────────────
std::map<std::string, double> VehicleData::snapshot() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return {
        { "EngineTemp",      engineTemp     },
        { "BatteryVoltage",  batteryVoltage },
        { "Speed",           speed          },
        { "TirePressure",    tirePressure   },
        { "DoorOpen",        doorOpen ? 1.0 : 0.0 },
        { "SeatbeltLocked",  seatbeltLocked ? 1.0 : 0.0 }
    };
}