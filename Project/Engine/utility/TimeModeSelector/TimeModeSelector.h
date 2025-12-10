#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class TimeMode {
    DELTA_TIME      = 0,
    DELTA_TIME_RATE = 1
};

namespace KetaEngine {

class GlobalParameter;

class TimeModeSelector {
public:
    TimeModeSelector()  = default;
    ~TimeModeSelector() = default;

    void RegisterParam(const std::string& groupName, GlobalParameter* globapParameter);
    void GetParam(const std::string& groupName, GlobalParameter* globapParameter);
    void SelectTimeModeImGui(const char* label);

private:
    int32_t timeModeInt_ = static_cast<int32_t>(TimeMode::DELTA_TIME_RATE);

    std::vector<const char*> TimeModeLabels = {
        "DeltaTime (No TimeScale)",
        "DeltaTimeRate (With TimeScale)"};

public:
    const int32_t GetTimeModeInt() const { return timeModeInt_; }
};

}; // KetaEngine
