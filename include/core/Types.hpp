// include/core/Types.hpp
#pragma once

namespace Thresholds {
    // Engine Temperature (°C)
    constexpr double ENGINE_TEMP_MIN      = 60.0;
    constexpr double ENGINE_TEMP_MAX      = 130.0;
    constexpr double ENGINE_TEMP_WARNING  = 100.0;
    constexpr double ENGINE_TEMP_CRITICAL = 115.0;

    // Battery Voltage (V)
    constexpr double BATTERY_MIN     = 10.0;
    constexpr double BATTERY_MAX     = 15.0;
    constexpr double BATTERY_WARNING = 11.5;
    constexpr double BATTERY_LOW     = 11.0;

    // Speed (km/h)
    constexpr double SPEED_MIN       = 0.0;
    constexpr double SPEED_MAX       = 200.0;
    constexpr double SPEED_CRITICAL  = 150.0;
    constexpr double SPEED_DOOR_WARN = 5.0;

    // Tire Pressure (PSI)
    constexpr double TIRE_MIN     = 15.0;
    constexpr double TIRE_MAX     = 40.0;
    constexpr double TIRE_WARNING = 25.0;
    constexpr double TIRE_LOW     = 20.0;
}