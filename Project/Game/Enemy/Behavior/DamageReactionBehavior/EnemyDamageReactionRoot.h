#pragma once

#include "BaseEnemyDamageReaction.h"

class PlayerAttackCollider;
class EnemyDamageReactionController;
class EnemyDamageReactionData;
class SideRope;

class EnemyDamageReactionRoot : public BaseEnemyDamageReaction {
public:
    // コンストラクタ
    EnemyDamageReactionRoot(BaseEnemy* boss);
    ~EnemyDamageReactionRoot();

    void Update(float deltaTime) override;
    void Debug() override;
    bool IsReactionRoot() const override { return true; }

    /// <summary>
    /// ダメージリアクションを再生
    /// </summary>
    /// <param name="playerCollisionInfo">プレイヤーの攻撃情報</param>
    /// <param name="skipAnimation">同一攻撃の再再生間隔未満の場合はアニメーションをスキップ</param>
    void SelectDamageActionBehaviorByAttack(const PlayerAttackCollider* playerCollisionInfo, bool skipAnimation = false);

private:
    /// <summary>
    /// 攻撃名からリアクションデータを取得して適用
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void ApplyReactionByAttackName(const std::string& attackName);

    /// <summary>
    /// 死亡リアクション変更
    /// </summary>
    /// <param name="reactionData">リアクションデータ</param>
    void ChangeDeathReaction(EnemyDamageReactionData* reactionData);

private:
    const PlayerAttackCollider* pPlayerCollisionInfo_    = nullptr;
    EnemyDamageReactionController* pReactionController_ = nullptr;
    bool skipAnimation_                                  = false;
};