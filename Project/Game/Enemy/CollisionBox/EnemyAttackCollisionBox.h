#pragma once

#include "CollisionBox/BaseAABBCollisionBox.h"
#include <array>
#include <string>

class BaseEnemy;
class Player;

/// <summary>
/// 敵の攻撃コリジョンボックス
/// </summary>
class EnemyAttackCollisionBox : public BaseAABBCollisionBox {
public:
    EnemyAttackCollisionBox()           = default;
    ~EnemyAttackCollisionBox() override = default;

    // 初期化、更新
    void Init() override;
    void Update() override;
    void TimerUpdate(float timeSpeed);

    /// <summary>
    /// 攻撃開始処理
    /// </summary>
    /// <param name="attackValue">攻撃力</param>
    /// <param name="collisionSize">コリジョンサイズ</param>
    /// <param name="offsetPos">オフセット位置</param>
    /// <param name="adaptTime">判定持続時間</param>
    void AttackStart(float attackValue, const Vector3& collisionSize, const Vector3& offsetPos, float adaptTime);

    /// <summary>
    /// オフセット位置を更新
    /// </summary>
    void UpdateOffset();

    /// <summary>
    /// 衝突中コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 衝突開始時コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;

private:
    // 敵
    BaseEnemy* pEnemy_ = nullptr;

    // 攻撃パラメータ
    float attackValue_;
    float adaptTimer_;
    Vector3 offsetPos_;

    // フラグ
    bool isHit_ = false;
    bool isFinish_ = false;

public:
    ///------------------------------------------------------------------------------
    /// getter method
    ///------------------------------------------------------------------------------
    Vector3 GetCollisionPos() const override;
    float GetAttackValue() const { return attackValue_; }
    const bool& GetIsHit() const { return isHit_; }
    const bool& GetIsFinish() const { return isFinish_; }

    ///------------------------------------------------------------------------------
    /// setter method
    ///------------------------------------------------------------------------------
    void SetEnemy(BaseEnemy* enemy) { pEnemy_ = enemy; }
    void SetIsHit(const bool& is) { isHit_ = is; }
};
