#pragma once

// baseEnemy
#include "BaseEnemy.h"

/// <summary>
/// 強敵クラス
/// </summary>
class StrongEnemy : public BaseEnemy {
public:
    StrongEnemy()           = default;
    ~StrongEnemy() override = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    void Init(const Vector3& spownPos) override;
    void Update() override;

    // スポーン演出
    void SpawnRenditionInit() override;

    /// <summary>
    /// 攻撃予備動作
    /// </summary>
    void AttackAnticipation() override;

    /// <summary>
    /// 攻撃予備動作が完了したか
    /// </summary>
    bool IsAttackAnticipationFinished() override;

    /// <summary>
    /// 攻撃開始処理
    /// </summary>
    void AttackStart() override;

    /// <summary>
    /// 攻撃更新処理
    /// </summary>
    void AttackUpdate() override;

    /// <summary>
    /// 攻撃が完了したか
    /// </summary>
    bool IsAttackFinished() override;

    /// <summary>
    /// 攻撃終了処理
    /// </summary>
    void AttackFinish() override;

    /// <summary>
    /// UIスプライトの表示
    /// </summary>
    /// <param name="viewProjection"></param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;

    /// <summary>
    /// 追跡開始時のアニメーション初期化
    /// </summary>
    void InitChaseAnimation() override;

    /// <summary>
    /// 追跡中のアニメーション更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    void UpdateChaseAnimation(float deltaTime) override;

    /// <summary>
    /// 待機アニメーションにリセット
    /// </summary>
    void ResetToWaitAnimation() override;

private:
    // 攻撃ステート用タイマー
    float attackAnticipationTimer_;
    bool isAttackAnticipationFinished_;
    bool isAttackFinished_;
};
