#pragma once

#include "BaseEnemyBehavior.h"
#include <functional>

class EnemyAttack : public BaseEnemyBehavior {
public:
    // 攻撃ステート
    enum class AttackState {
        ANTICIPATION, // 予備動作
        START,        // 開始
        UPDATE,       // 攻撃更新
        FINISH        // 終了処理
    };

public:
    // コンストラクタ
    EnemyAttack(BaseEnemy* boss);
    ~EnemyAttack();

    void Update() override;
    void Debug() override;

private:
    /// <summary>
    /// ステートを変更
    /// </summary>
    /// <param name="newState">新しいステート</param>
    void ChangeState(AttackState newState);

    /// <summary>
    /// 現在のステート処理を実行
    /// </summary>
    void ExecuteCurrentState();

private:
    // 現在のステート
    AttackState currentState_;

    // ステート処理関数
    std::function<void()> currentStateFunction_;

    // タイマー
    float stateTimer_;
};
