#pragma once
#include "utility/ParameterEditor/GlobalParameter.h"
#include <string>

class TimeScaleData {
public:
    TimeScaleData()  = default;
    ~TimeScaleData() = default;

    /// 初期化
    void Init(const std::string& timeScaleName);

    /// ImGuiでの調整
    void AdjustParam();

    /// データのロード・セーブ
    void LoadData();
    void SaveData();

private:
    /// パラメータのバインド
    void BindParams();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "TimeScale";

    // TimeScaleパラメータ
    float timeScale_ = 1.0f;
    float duration_  = 1.0f;

    // UI表示制御
    bool showControls_ = true;

public:
    // ゲッター・セッター
    std::string GetGroupName() const { return groupName_; }
    float GetTimeScale() const { return timeScale_; }
    float GetDuration() const { return duration_; }
    void SetTimeScale(float timeScale) { timeScale_ = timeScale; }
    void SetDuration(float duration) { duration_ = duration; }
};