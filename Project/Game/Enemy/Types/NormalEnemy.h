#pragma once
#include <list>
#include <memory>
// baseEnemy
#include "BaseEnemy.h"

/// <summary>
/// 通常敵クラス
/// </summary>
class NormalEnemy : public BaseEnemy {
public:
    NormalEnemy()           = default;
    ~NormalEnemy() override = default;

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
    /// スプライト表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection) override;

private:
    // 攻撃ステート用タイマー
    float attackAnticipationTimer_;
    bool isAttackAnticipationFinished_;
    bool isAttackFinished_;
};
