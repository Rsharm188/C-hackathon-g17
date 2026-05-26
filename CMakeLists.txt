cmake_minimum_required(VERSION 3.14)
project(VehicleMonitoring CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Compiler warnings ──────────────────────────────────────────────────────────
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ── Source files ───────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp

    src/core/Sensor.cpp
    src/core/Alert.cpp
    src/core/VehicleData.cpp

    src/sensors/EngineTemperatureSensor.cpp
    src/sensors/BatterySensor.cpp
    src/sensors/SpeedSensor.cpp
    src/sensors/TirePressureSensor.cpp
    src/sensors/DoorSensor.cpp
    src/sensors/SeatbeltSensor.cpp

    src/managers/SensorManager.cpp
    src/managers/AlertManager.cpp
    src/managers/ThreadManager.cpp

    src/display/Dashboard.cpp
    src/logging/EventLogger.cpp
    src/utils/Statistics.cpp
)

# ── Include directories ────────────────────────────────────────────────────────
include_directories(include)

# ── Executable ────────────────────────────────────────────────────────────────
add_executable(vehicle_monitor ${SOURCES})

# ── Link pthreads ─────────────────────────────────────────────────────────────
find_package(Threads REQUIRED)
target_link_libraries(vehicle_monitor PRIVATE Threads::Threads)

# ── Create logs/ dir at build time if missing ─────────────────────────────────
add_custom_command(TARGET vehicle_monitor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:vehicle_monitor>/logs)