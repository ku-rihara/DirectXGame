#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "KillCountUIController.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <string>

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
    void Update();
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

    KillCountUIController killCountUIController_;

    PlayerComboAttackController* pAttackController_ = nullptr;
    std::function<void(const std::string&)> onAttackUnlockedCallback_;


    // GlobalParameter
    KetaEngine::GlobalParameter* globalParameter_ = nullptr;
    const std::string groupName_ = "KillCounter";

public:
    // Getter
    int32_t GetCurrentLevel() const { return currentLevel_; }
    int32_t GetKillCount() const { return killCount_; }
    // ableDefeatLevel に対応するロック解除までの残りキル数
    int32_t GetRemainingKillsForLevel(int32_t ableDefeatLevel) const {
        if (ableDefeatLevel <= 0 || ableDefeatLevel > kMaxKillLevel) { return 0; }
        return (std::max)(0, levelUpThresholds_[ableDefeatLevel - 1] - killCount_);
    }
    // Setter
    void SetAttackController(PlayerComboAttackController* controller) { pAttackController_ = controller; }
    void SetOnAttackUnlockedCallback(std::function<void(const std::string&)> callback) { onAttackUnlockedCallback_ = std::move(callback); }

};
