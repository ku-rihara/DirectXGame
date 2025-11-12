#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <string>

/// <summary>
/// タイムスケールデータ
/// </summary>
class TimeScaleData {
public:
    TimeScaleData()  = default;
    ~TimeScaleData() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="timeScaleName">タイムスケール名</param>
    void Init(const std::string& timeScaleName);

    void AdjustParam(); //< パラメータ調整
    void LoadData(); //< データロード
    void SaveData(); //< データセーブ

private:
    void RegisterParams(); //< パラメータのバインド

private:
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "TimeScale";

    float timeScale_ = 1.0f;
    float duration_  = 1.0f;

    bool showControls_ = true;

public:
    // getter
    const std::string& GetGroupName() const { return groupName_; }
    const float& GetTimeScale() const { return timeScale_; }
    const float& GetDuration() const { return duration_; }

    // setter
    void SetTimeScale(const float& timeScale) { timeScale_ = timeScale; }
    void SetDuration(const float& duration) { duration_ = duration; }
};