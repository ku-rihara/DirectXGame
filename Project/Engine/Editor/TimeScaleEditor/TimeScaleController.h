#pragma once
#include "TimeScaleData.h"
#include <memory>
#include <string>
#include <vector>

namespace KetaEngine {

class TimeScaleController {
public:
    TimeScaleController()  = default;
    ~TimeScaleController() = default;

    /// 初期化,更新
    void Init();
    void Update(float deltaTime);

    /// TimeScale制御
    void PlayTimeScale(const std::string& timeScaleName);
    void StopTimeScale();
    void SetTimeScaleImmediate(float timeScale);

    /// 状態取得
    float GetCurrentTimeScale() const;
    bool IsTimeScaleActive() const;

    /// エディタ機能
    void EditorUpdate();
    void AddTimeScale(const std::string& timeScaleName);
    void RemoveTimeScale(int index);
    void AllLoadFile();
    void AllSaveFile();

    /// 取得
    TimeScaleData* GetTimeScaleByName(const std::string& name);
    int GetTimeScaleCount() const { return static_cast<int>(timeScales_.size()); }

private:
    std::vector<std::unique_ptr<TimeScaleData>> timeScales_;
    int selectedIndex_ = -1;

    // 入力用バッファ
    char nameBuffer_[128] = "";

    // 時間管理
    bool isTimeScaleActive_ = false;
    float currentTimer_     = 0.0f;
    float targetDuration_   = 0.0f;
};

}; // KetaEngine
