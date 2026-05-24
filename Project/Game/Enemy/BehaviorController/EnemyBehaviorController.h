#pragma once
#include <memory>
#include <string>

class BaseEnemy;
class BaseEnemyBehavior;
class BaseEnemyDamageReaction;
class PlayerAttackCollider;

/// <summary>
/// 敵のビヘイビア管理クラス
/// </summary>
class EnemyBehaviorController {
public:
    EnemyBehaviorController() = default;
    ~EnemyBehaviorController();

    void Init(BaseEnemy* owner);

    /// <summary>
    /// 毎フレーム更新
    /// </summary>
    void Update(float deltaTime);

    /// <summary>
    /// 移動ビヘイビア変更
    /// </summary>
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);

    /// <summary>
    /// ダメージリアクションビヘイビア変更
    /// </summary>
    void ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior);

    /// <summary>
    /// プレイヤー攻撃コライダーとの衝突処理
    /// </summary>
    void OnPlayerAttackCollision(PlayerAttackCollider* attackController);

    void StartDamageColling(float time, const std::string& attackName);
    void ResetDamageCooling();

    /// <summary>
    /// プール返却前にビヘイビアを破棄
    /// </summary>
    void Reset();

    BaseEnemyBehavior*          GetMoveBehavior()    const { return moveBehavior_.get(); }
    BaseEnemyDamageReaction*    GetDamageBehavior()  const { return damageBehavior_.get(); }
    bool                        IsDamageColling()    const { return isDamageColling_; }
    const std::string&          GetLastAttackName()  const { return lastReceivedAttackName_; }

private:
    void DamageCollingUpdate(float deltaTime);

    /// <summary>
    /// ビヘイビア変更を禁止すべき状態かどうか
    /// </summary>
    bool IsChangeLocked() const;

    BaseEnemy* pOwner_ = nullptr;

    std::unique_ptr<BaseEnemyBehavior>       moveBehavior_;
    std::unique_ptr<BaseEnemyDamageReaction> damageBehavior_;

    bool        isDamageColling_        = false;
    float       damageCollTime_         = 0.0f;
    std::string lastReceivedAttackName_;
};
