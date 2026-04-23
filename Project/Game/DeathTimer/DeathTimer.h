#pragma once

#include "Combo/ComboUIController.h"
#include "DeathTimer/DeathTimerGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "UI/LevelUI/LevelUIController.h"
// std
#include <array>
#include <cstdint>
#include <functional>
#include <memory>

/// <summary>
/// プレイヤーHP管理クラス
/// </summary>
class DeathTimer {
public:
    static constexpr int32_t kMaxLevel = 6;

public:
    DeathTimer()  = default;
    ~DeathTimer() = default;

    // 初期化、更新
    void Init();
    void Update(float deltaTime);

    void AdjustParam();
    void RegisterParams();

    void OnEnemyKilled(float gaugeAmount, int32_t comboCount);
    void TakeDamage(float deltaTime);

    // ゲージ直接回復（シムキルボーナスなど外部から）
    void ApplyBonus(float amount);

private:
    void RecoverHP(float amount);
    void AdaptEasing();
    void UpdateLevel();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    std::unique_ptr<DeathTimerGauge> deathTimerGauge_;
    LevelUIController levelUIController_;

    float currentHP_ = 0.0f;
    float maxHP_;

    // レベル別減少レート（レベル1〜6）
    std::array<float, kMaxLevel> decreaseRates_;

    // レベルアップキル数閾値（レベル2〜6に上がるキル数、累計）
    std::array<int32_t, kMaxLevel - 1> levelUpKillCounts_;

    // 現在のゲージ減りレベル（1〜6）
    int32_t currentLevel_ = 1;

    // 累計キル数
    int32_t killCount_ = 0;

    // コンボ倍率パラメータ
    int32_t comboStepSize_          = 10;   // 何コンボおきに倍率が上がるか
    float comboMultiplierPerStep_   = 1.5f; // 1ステップごとの倍率
    float comboMaxMultiplier_       = 5.0f; // 倍率の上限

    // イージング関連
    bool isRecovering_         = false;
    float recoveryStartValue_  = 0.0f;
    float recoveryTargetValue_ = 0.0f;
    float recoveryTimer_       = 0.0f;
    float recoveryDuration_    = 0.0f;

    bool isDeath_ = false;

#if defined(_DEBUG) || defined(DEVELOPMENT)
    bool isGodMode_ = true; // デバッグ用: HP減少を止める（初期ON）
#endif

    // キル通知コールバック（comboMultiplier: float）
    std::function<void(float)> onKillCallback_;

public:
    const bool& GetIsDeath() const { return isDeath_; }
    void SetOnKillCallback(std::function<void(float)> cb) { onKillCallback_ = std::move(cb); }
    float GetCurrentHP() const { return currentHP_; }
    float GetMaxHP() const { return maxHP_; }
    int32_t GetCurrentLevel() const { return currentLevel_; }
    int32_t GetKillCount() const { return killCount_; }
    DeathTimerGauge* GetDeathTimerGauge() const { return deathTimerGauge_.get(); }
};
