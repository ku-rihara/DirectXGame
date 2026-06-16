#pragma once

#include "StressGauge/StressGaugeGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <functional>
#include <memory>

/// <summary>
/// ストレスゲージ管理クラス
/// </summary>
class StressGauge {
public:
    StressGauge()  = default;
    ~StressGauge() = default;

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
    void OnEntourageEnemyHit();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "StressGauge";

    std::unique_ptr<StressGaugeGauge> StressGaugeGauge_;

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
    StressGaugeGauge* GetStressGaugeGauge() const { return StressGaugeGauge_.get(); }

    void SetOnDecayTickCallback(std::function<void()> cb) { onDecayTick_ = std::move(cb); }
};
