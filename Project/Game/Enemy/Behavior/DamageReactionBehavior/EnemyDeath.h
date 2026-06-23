#pragma once

#include "BaseEnemyDamageReaction.h"
#include <functional>
#include <map>

class EnemyDeath : public BaseEnemyDamageReaction {
public:
    EnemyDeath(BaseEnemy* boss);
    ~EnemyDeath();
    void AngleCaluclation();

    void Update(float deltaTime) override;
    void Debug() override;

private:
    // 死亡処理のステップ
    enum class Step {
        DIRECTIONSET, // プレイヤー方向を向く
        ANIMATION, // 死亡アニメーション再生
        BURST, // 爆散処理
        WAITING, // エフェクト待機
        DEATH, // 死亡フラグ設定
    };

private:
    // ステップごとの処理
    void StepDirectionSet();
    void StepAnimation();
    void StepBurst();
    void StepWaiting();
    void StepDeath();

    // ステップ関数のセットアップ
    void SetUpSteps();

private:
    Step step_              = Step::DIRECTIONSET;
    bool deathAnimStarted_  = false;
    bool deathAnimFinished_ = false;
    float deathWaitTimer_   = 0.0f;

    std::map<Step, std::function<void()>> stepPhase_;
};
