#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <cstdint>

class PlayerComboAttackController;

/// <summary>
/// 敵撃破数に基づくレベル管理クラス
/// </summary>
class KillCounter {
public:
    static constexpr int32_t kMaxKillLevel = 5;

public:
    KillCounter()  = default;
    ~KillCounter() = default;

    void Init();
    void AddKillCount();

    // ImGui / GlobalParameter
    void AdjustParam();
    void RegisterParams();

private:
    void CheckLevelUp();
    void UnlockAttacksForLevel(int32_t level);

private:
    int32_t killCount_    = 0;
    int32_t currentLevel_ = 0;
    int32_t levelUpThresholds_[kMaxKillLevel] = {};

    PlayerComboAttackController* pAttackController_ = nullptr;


    // GlobalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "KillCounter";

public:
    // Getter
    int32_t GetCurrentLevel() const { return currentLevel_; }
    int32_t GetKillCount() const { return killCount_; }
    // Setter
    void SetAttackController(PlayerComboAttackController* controller) { pAttackController_ = controller; }

};
