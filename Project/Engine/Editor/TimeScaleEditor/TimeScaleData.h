#pragma once
#include "Easing/Easing.h"
#include "Editor/BaseEffectEditor/BaseEffectData.h"
#include "utility/TimeModeSelector/TimeModeSelector.h"
#include <cstdint>
#include <string>

/// <summary>
/// タイムスケールデータ
/// </summary>
namespace KetaEngine {

class TimeScaleData : public BaseEffectData {
public:
    TimeScaleData()           = default;
    ~TimeScaleData() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectDataからのオーバーライド
    void Init(const std::string& timeScaleName) override;
    void Update(float deltaTime) override;
    void Reset() override;
    void Play() override;
    void LoadData() override;
    void SaveData() override;

    // TimeScale固有
    void AdjustParam();

private:
    //*---------------------------- private Methods ----------------------------*//

    // BaseEffectDataからのオーバーライド
    void RegisterParams() override;
    void GetParams() override;
    void InitParams() override;

    // TimeScale制御
    void ApplyTimeScale();
    void ResetTimeScale();

private:
    //*---------------------------- private Variant ----------------------------*//

    float timeScale_   = 1.0f;
    float duration_    = 1.0f;
    float elapsedTime_ = 0.0f;

    TimeModeSelector timeModeSelector_;

public:
    //*----------------------------- getter Methods -----------------------------*//

    float GetTimeScale() const { return timeScale_; }
    float GetDuration() const { return duration_; }
    float GetElapsedTime() const { return elapsedTime_; }
    float GetRemainingTime() const { return duration_ - elapsedTime_; }

    //*----------------------------- setter Methods -----------------------------*//

    void SetTimeScale(float timeScale) { timeScale_ = timeScale; }
    void SetDuration(float duration) { duration_ = duration; }
};

}; // KetaEngine