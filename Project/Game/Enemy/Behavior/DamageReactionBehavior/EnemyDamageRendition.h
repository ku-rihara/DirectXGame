#pragma once

#include "Enemy/DamageReaction/EnemyDamageReactionData.h"
#include <cstdint>

class BaseEnemy;
class EnemyDamageRenditionData;

/// <summary>
/// 敵ダメージ演出制御クラス
/// </summary>
class EnemyDamageRendition {
public:
    EnemyDamageRendition()  = default;
    ~EnemyDamageRendition() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="enemy">敵オブジェクト</param>
    /// <param name="reactionData">リアクションデータ</param>
    void Init(BaseEnemy* enemy, EnemyDamageReactionData* reactionData);

    /// <summary>
    /// 更新
    /// </summary>
    /// <param name="deltaTime">デルタタイム</param>
    /// <param name="reactionTimer">リアクションタイマー</param>
    /// <param name="hasPlayedRendition">演出再生済みフラグ</param>
    void Update(float deltaTime, float reactionTimer, bool& hasPlayedRendition);

    /// <summary>
    /// リセット
    /// </summary>
    void Reset();

private:
    BaseEnemy* pBaseEnemy_                  = nullptr;
    EnemyDamageReactionData* pReactionData_ = nullptr;

    float currentTime_ = 0.0f;
};