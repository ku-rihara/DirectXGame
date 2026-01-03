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

    void Update(float deltaTime) override;
    void Debug() override;

    /// <summary>
    /// ダメージリアクションを再生
    /// </summary>
    /// <param name="playerCollisionInfo">プレイヤーの攻撃情報</param>
    void SelectDamageActionBehaviorByAttack(const PlayerCollisionInfo* playerCollisionInfo);

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

    /// <summary>
    /// ダメージパーティクルエフェクトを再生
    /// </summary>
    /// <param name="reactionData">リアクションデータ</param>
    void PlayDamageParticleEffect(EnemyDamageReactionData* reactionData);

private:
    const PlayerCollisionInfo* pPlayerCollisionInfo_    = nullptr;
    EnemyDamageReactionController* pReactionController_ = nullptr;
};