#pragma once

#include "DeathTimer/DeathTimerGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <functional>
#include <memory>

/// <summary>
/// ストレスゲージ管理クラス
/// </summary>
class DeathTimer {
public:
    DeathTimer()  = default;
    ~DeathTimer() = default;

    // 初期化、更新
    void Init();
    void Update(float deltaTime);

    void AdjustParam();
    void RegisterParams();

    /// <summary>
    /// 敵の攻撃を受けた時に呼ぶ
    /// </summary>
    void TakeDamage(float amount);

    /// <summary>
    /// CrawlBackwards状態の敵へ攻撃が当たった時に呼ぶ
    /// </summary>
    void OnNormalEnemyHit();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    std::unique_ptr<DeathTimerGauge> deathTimerGauge_;

    // ストレス量（0 → maxStress_ でゲームオーバー）
    float currentStress_ = 0.0f;
    float maxStress_     = 100.0f;

    // ストレス減少
    float stressReductionPerHit_ = 5.0f; // ザコ攻撃1回あたりの減少量

    std::function<void()> onDecayTick_;

    bool isDeath_   = false;
    bool isGodMode_ = false; // デバッグ用: ストレス増加を止める

public:
    const bool& GetIsDeath() const { return isDeath_; }
    bool& GetIsDeathRef() { return isDeath_; }
    float GetCurrentStress() const { return currentStress_; }
    float GetMaxStress() const { return maxStress_; }
    DeathTimerGauge* GetDeathTimerGauge() const { return deathTimerGauge_.get(); }

    void SetOnDecayTickCallback(std::function<void()> cb) { onDecayTick_ = std::move(cb); }
};
