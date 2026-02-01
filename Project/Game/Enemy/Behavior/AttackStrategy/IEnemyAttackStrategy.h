#pragma once

class BaseEnemy;

/// <summary>
/// 敵の攻撃動作を定義するインターフェース
/// </summary>
class IEnemyAttackStrategy {
public:
    IEnemyAttackStrategy(BaseEnemy* enemy) : pEnemy_(enemy) {}
    virtual ~IEnemyAttackStrategy() = default;

    /// <summary>
    /// 攻撃予備動作
    /// </summary>
    virtual void Anticipation() = 0;

    /// <summary>
    /// 攻撃予備動作が完了したか
    /// </summary>
    virtual bool IsAnticipationFinished() const = 0;

    /// <summary>
    /// 攻撃開始処理
    /// </summary>
    virtual void Start() = 0;

    /// <summary>
    /// 攻撃更新処理
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// 攻撃が完了したか
    /// </summary>
    virtual bool IsFinished() const = 0;

    /// <summary>
    /// 攻撃終了処理
    /// </summary>
    virtual void Finish() = 0;

    /// <summary>
    /// 状態リセット
    /// </summary>
    virtual void Reset() {
        anticipationTimer_ = 0.0f;
        isAnticipationFinished_ = false;
        isAttackFinished_ = false;
        hasPlayedAnticipationEffect_ = false;
    }

protected:
    BaseEnemy* pEnemy_ = nullptr;
    float anticipationTimer_ = 0.0f;
    bool isAnticipationFinished_ = false;
    bool isAttackFinished_ = false;
    bool hasPlayedAnticipationEffect_ = false;
};
