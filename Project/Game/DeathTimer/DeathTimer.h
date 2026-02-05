#pragma once

#include "Combo/ComboUIController.h"
#include "DeathTimer/DeathTimerGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <memory>

/// <summary>
/// プレイヤーHP管理クラス
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

    /// <summary>
    /// ダメージを受ける
    /// </summary>
    /// <param name="damage">ダメージ量</param>
    void TakeDamage(float damage);

    /// <summary>
    /// HPを回復する（コンボレベルアップ時）
    /// </summary>
    void RecoverHP();

private:
    // イージング適応
    void AdaptEasing();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    std::unique_ptr<DeathTimerGauge> deathTimerGauge_;

    float currentHP_ = 0.0f;
    float maxHP_;

    // 回復量（コンボレベルアップ時）
    float levelUpRecoveryAmount_;

    // イージング関連
    bool isRecovering_        = false;
    float recoveryStartValue_  = 0.0f;
    float recoveryTargetValue_ = 0.0f;
    float recoveryTimer_       = 0.0f;
    float recoveryDuration_    = 0.0f;

    bool isDeath_ = false;

public:
    // Getter
    const bool& GetIsDeath() const { return isDeath_; }
    float GetCurrentHP() const { return currentHP_; }
    float GetMaxHP() const { return maxHP_; }
    DeathTimerGauge* GetDeathTimerGauge() const { return deathTimerGauge_.get(); }
};