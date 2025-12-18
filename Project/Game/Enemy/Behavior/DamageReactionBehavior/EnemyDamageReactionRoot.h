#pragma once

#include "BaseEnemyDamageReaction.h"

class PlayerCollisionInfo;
class EnemyDamageReactionController;
class EnemyDamageReactionData;
class SideRope;

class EnemyDamageReactionRoot : public BaseEnemyDamageReaction {
public:
    // コンストラクタ
    EnemyDamageReactionRoot(BaseEnemy* boss);
    ~EnemyDamageReactionRoot();

    void Update() override;
    void Debug() override;

    /// <summary>
    /// プレイヤーの攻撃情報からダメージリアクションを選択して実行
    /// </summary>
    /// <param name="playerCollisionInfo">プレイヤーの攻撃情報</param>
    void SelectDamageActionBehaviorByAttack(const PlayerCollisionInfo* playerCollisionInfo);

private:
    /// <summary>
    /// 攻撃名からリアクションデータを取得して適用
    /// </summary>
    /// <param name="attackName">攻撃名</param>
    void ApplyReactionByAttackName(const std::string& attackName);

    // 死亡リアクション変更
    void ChangeDeathReaction(EnemyDamageReactionData* reactionData);
    void ChangeRopeBoundReaction(SideRope*sideRope);

private:
    const PlayerCollisionInfo* pPlayerCollisionInfo_    = nullptr;
    EnemyDamageReactionController* pReactionController_ = nullptr;
};