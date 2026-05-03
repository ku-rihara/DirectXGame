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
    /// 煽り状態をセット
    /// </summary>
    /// <param name="isTaunting">ボスがトリガー距離内にいるか</param>
    /// <param name="tauntingCount">現在煽り中のザコ数</param>
    void SetTauntState(bool isTaunting, int32_t tauntingCount);

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

    // 煽り中のストレス増加（秒方式）
    float tauntTickInterval_  = 1.0f; // 何秒間隔で1回増加するか
    float tauntTickTimer_     = 0.0f; // 秒用タイマー
    float baseStressRate_     = 5.0f; // 基準増加量（秒ごと）
    float stressRatePerEnemy_ = 2.0f; // ザコ1体あたりの追加増加量（秒ごと）

    // ボス撃破後のストレス減少
    float stressReductionPerHit_ = 5.0f; // ザコ攻撃1回あたりの減少量

    // コールバック
    std::function<void()> onDecayTick_; // ストレス減少時に呼ばれる

    // 煽り状態
    bool isTaunting_            = false;
    int32_t tauntingEnemyCount_ = 0;

    bool isDeath_ = false;

    bool isGodMode_ = true; // デバッグ用: ストレス増加を止める

public:
    const bool& GetIsDeath() const { return isDeath_; }
    float GetCurrentStress() const { return currentStress_; }
    float GetMaxStress() const { return maxStress_; }
    DeathTimerGauge* GetDeathTimerGauge() const { return deathTimerGauge_.get(); }

    // コールバック設定
    void SetOnDecayTickCallback(std::function<void()> cb) { onDecayTick_ = std::move(cb); }
};
