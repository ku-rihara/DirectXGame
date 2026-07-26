#pragma once

#include "BaseEnemyDamageReaction.h"
#include <functional>
#include <map>

/// <summary>
/// 敵の死亡演出(方向設定→アニメーション→爆散→待機→死亡確定)を制御するリアクション
/// </summary>
class EnemyDeath : public BaseEnemyDamageReaction {
public:
   
    EnemyDeath(BaseEnemy* boss, bool startFromBurst = false);
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
    float deathAnimTimer_   = 0.0f;
    float deathWaitTimer_   = 0.0f;

    std::map<Step, std::function<void()>> stepPhase_;
};
