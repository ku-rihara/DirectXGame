#pragma once

#include "DeathTimer/DeathTimerGauge.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <cstdint>
#include <functional>
#include <memory>

/// <summary>
/// ストレスゲージ管理クラス
/// - ボスがプレイヤーに近づきザコが煽ると増加
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
    /// ボスが倒された瞬間に呼ぶ
    /// </summary>
    void OnBossDead();

    /// <summary>
    /// ボス撃破後にザコへ攻撃が当たった時に呼ぶ
    /// </summary>
    void OnNormalEnemyHit();

private:
    KetaEngine::GlobalParameter* globalParameter_;
    const std::string groupName_ = "DeathTimer";

    std::unique_ptr<DeathTimerGauge> deathTimerGauge_;

    // ストレス量（0 → maxStress_ でゲームオーバー）
    float currentStress_ = 0.0f;
    float maxStress_     = 100.0f;

    // 煽り中のストレス増加（チック方式）
    float tauntTickInterval_    = 1.0f;  // 何秒間隔で1回増加するか
    float tauntTickTimer_       = 0.0f;  // チック用タイマー
    float baseStressRate_       = 5.0f;  // 基準増加量（チックごと）
    float stressRatePerEnemy_   = 2.0f;  // ザコ1体あたりの追加増加量（チックごと）

    // ボス撃破後のストレス減少
    float decayTickInterval_     = 1.0f;   // 何秒ごとに1回減らすか
    float decayAmountPerTick_    = 3.0f;   // 1チックで減るストレス量
    float decayTickTimer_        = 0.0f;   // チック用タイマー
    float stressReductionPerHit_ = 5.0f;   // ザコ攻撃1回あたりの減少量

    // コールバック（ボス位置エフェクト・UI等に使う）
    std::function<void()> onDecayTick_;    // ストレス減少チック時に呼ばれる
    std::function<void()> onBossKilled_;   // ボス撃破時に呼ばれる

    // 煽り状態
    bool    isTaunting_         = false;
    int32_t tauntingEnemyCount_ = 0;

    bool isBossDead_ = false;
    bool isDeath_    = false;

#if defined(_DEBUG) || defined(DEVELOPMENT)
    bool isGodMode_ = true; // デバッグ用: ストレス増加を止める
#endif

public:
    const bool& GetIsDeath() const { return isDeath_; }
    float GetCurrentStress() const { return currentStress_; }
    float GetMaxStress() const { return maxStress_; }
    bool GetIsBossDead() const { return isBossDead_; }
    DeathTimerGauge* GetDeathTimerGauge() const { return deathTimerGauge_.get(); }

    // コールバック設定
    void SetOnDecayTickCallback(std::function<void()> cb) { onDecayTick_  = std::move(cb); }
    void SetOnBossKilledCallback(std::function<void()> cb) { onBossKilled_ = std::move(cb); }
};
