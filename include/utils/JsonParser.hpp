// include/utils/JsonParser.hpp
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include<algorithm>
struct TestCase {
    double engineTemp;
    double batteryVoltage;
    double speed;
    double tirePressure;
    bool   doorOpen;
    bool   seatbeltLocked;
};

inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

inline std::string extractValue(const std::string& block, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t pos = block.find(search);
    if (pos == std::string::npos)
        throw std::runtime_error("Key not found: " + key);
    pos = block.find(':', pos);
    size_t end = block.find_first_of(",}", pos);
    return trim(block.substr(pos + 1, end - pos - 1));
}

inline bool   parseBool  (const std::string& val) { return trim(val) == "true"; }
inline double parseDouble(const std::string& val) { return std::stod(trim(val)); }

inline std::vector<TestCase> loadTestCases(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open test file: " + filepath);

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string content = ss.str();

    std::vector<TestCase> cases;
    size_t pos = 0;
    while ((pos = content.find('{', pos)) != std::string::npos) {
        size_t end = content.find('}', pos);
        if (end == std::string::npos) break;
        std::string block = content.substr(pos, end - pos + 1);

        TestCase tc;
        tc.engineTemp     = parseDouble(extractValue(block, "engineTemp"));
        tc.batteryVoltage = parseDouble(extractValue(block, "batteryVoltage"));
        tc.speed          = parseDouble(extractValue(block, "speed"));
        tc.tirePressure   = parseDouble(extractValue(block, "tirePressure"));
        tc.doorOpen       = parseBool(extractValue(block, "doorOpen"));
        tc.seatbeltLocked = parseBool(extractValue(block, "seatbeltLocked"));

        cases.push_back(tc);
        pos = end + 1;
    }

    if (cases.empty())
        throw std::runtime_error("No test cases found in: " + filepath);

    return cases;
}

// Lists all .json files in the tests/ folder
inline std::vector<std::string> listTestFiles(const std::string& folder = "tests") {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() == ".json")
            files.push_back(entry.path().stem().string()); // just filename without .json
    }
    std::sort(files.begin(), files.end());
    return files;
}