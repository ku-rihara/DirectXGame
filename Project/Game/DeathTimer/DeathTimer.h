#pragma once

#include "Combo/ComboUIController.h"
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
    void Update();

    void AdjustParam();
    void RegisterParams();

private:
    void DebugMode();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    float currentTimer_ = 0.0f;

    float incrementTime_           = 0.0f;
    float incrementByComboLevelUP_ = 0.0f;
    float startTimer_              = 0.0f;

public:
    // Getter
};