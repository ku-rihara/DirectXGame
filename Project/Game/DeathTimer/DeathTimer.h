#pragma once

#include "Combo/ComboUIController.h"
#include"DeathTimer/DeathTimerGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

/// <summary>
/// デスタイマー
/// </summary>
class DeathTimer {
public:
    DeathTimer()  = default;
    ~DeathTimer() = default;

    // 初期化、更新
    void Init();
    void Update(float timer);

    void AdjustParam();
    void RegisterParams();

private:
    void DebugMode();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    std::unique_ptr<DeathTimerGauge> deathTimerGauge_;

    float currentTimer_ = 0.0f;

    float decrementSpeedRate_ = 1.0f;
    float incrementTime_;
    float incrementByComboLevelUP_;
    float maxTimer_;

public:
    // Getter
};