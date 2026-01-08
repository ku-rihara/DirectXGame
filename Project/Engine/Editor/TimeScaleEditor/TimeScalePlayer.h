#pragma once
#include "Editor/BaseEffectEditor/BaseEffectPlayer.h"
#include "TimeScaleData.h"
#include <memory>
#include <string>

/// <summary>
/// タイムスケールプレイヤー
/// </summary>
namespace KetaEngine {

class TimeScalePlayer : public BaseEffectPlayer {
public:
    TimeScalePlayer()           = default;
    ~TimeScalePlayer() override = default;

    //*----------------------------- public Methods -----------------------------*//

    // BaseEffectPlayerからのオーバーライド
    void Init() override;
    void Update(float deltaTime) override;
    void Play(const std::string& timeScaleName) override;

    // TimeScale固有
    void SetTimeScaleImmediate(float timeScale);
    float GetCurrentTimeScale() const;

private:
    //*---------------------------- private Methods ----------------------------*//

    std::unique_ptr<BaseEffectData> CreateEffectData() override;

public:
    //*----------------------------- getter Methods -----------------------------*//

    float GetElapsedTime() const;
    float GetRemainingTime() const;
};

}; // KetaEngine