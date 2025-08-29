#pragma once
#include "Easing/Easing.h"
#include "utility/ParameterEditor/GlobalParameter.h"
#include <cstdint>
#include <string>

class DissolveData {
public:
    enum class PlayState {
        STOPPED,
        PLAYING,
        PAUSED
    };

public:
    DissolveData()  = default;
    ~DissolveData() = default;

    /// 初期化、更新、調節
    void Init(const std::string& dissolveName);
    void Update(float deltaTime);
    void AdjustParam();

    /// 再生制御
    void Play();
    void Stop();
    void Reset();

    /// Load,Save
    void LoadData();
    void SaveData();

    /// 状態取得
    bool IsPlaying() const;
    bool IsFinished() const;

private:
    /// Editor
    void BindParams();
    void EasingTypeSelector(const char* label, int32_t& target);

    /// ディゾルブ値の計算
    void UpdateDissolveValues();

private:
    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string folderPath_ = "DissolveEditor";

    // ディゾルブパラメータ
    float startThreshold_ = 1.0f;
    float maxTime_        = 1.0f;
    float endThreshold_;
    float offsetTime_;
    int32_t easeType_ = 0;

    // 再生状態
    PlayState playState_ = PlayState::STOPPED;
    float currentTime_   = 0.0f;
    float totalTime_     = 0.0f;

    // 現在のディゾルブ値
    float currentThreshold_ = 1.0f;
    bool currentEnable_     = false;

    // イージング用
    Easing<float> thresholdEase_;
    float easedThreshold_ = 1.0f;

    // UI表示
    bool showControls_ = true;

public:
    //--------------------------------------------------------------------------------------
    // getter
    //--------------------------------------------------------------------------------------
    std::string GetGroupName() const { return groupName_; }
    float GetCurrentThreshold() const { return currentThreshold_; }
    bool IsDissolveEnabled() const { return currentEnable_; }
    float GetStartThreshold() const { return startThreshold_; }
    float GetEndThreshold() const { return endThreshold_; }
    float GetMaxTime() const { return maxTime_; }
    float GetOffsetTime() const { return offsetTime_; }

    //--------------------------------------------------------------------------------------
    // setter
    //--------------------------------------------------------------------------------------
    void SetStartThreshold(float threshold) { startThreshold_ = threshold; }
    void SetEndThreshold(float threshold) { endThreshold_ = threshold; }
    void SetMaxTime(float time) { maxTime_ = time; }
    void SetOffsetTime(float time) { offsetTime_ = time; }
    void SetEaseType(int32_t type) { easeType_ = type; }
};