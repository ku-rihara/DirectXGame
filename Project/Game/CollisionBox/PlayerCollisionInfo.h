#pragma once

#include "BaseAABBCollisionBox.h"
#include "Combo/Combo.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include <array>
#include <string>

class PlayerComboAttackData;
/// <summary>
/// プレイヤー攻撃の制御クラス
/// </summary>
class PlayerCollisionInfo : public BaseAABBCollisionBox {


public:
    PlayerCollisionInfo()           = default;
    ~PlayerCollisionInfo() override = default;

    // 初期化、更新、描画
    void Init() override;
    void Update() override;
    void TimerUpdate(float timeSpeed);

    void UpdateOffset();

    // 攻撃情報取得
    void AttackStart(const PlayerComboAttackData* comboAttackData);

    // コリジョンコールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

private:
    void LoopWaiting(float timeSpeed);
    void LoopStart();
  
private:
    const WorldTransform* baseTransform_          = nullptr;
    const PlayerComboAttackData* comboAttackData_ = nullptr;

    // time
    float adaptTimer_;
    float loopWaitTimer_;
    bool hasHitEnemy_ = false;

    // count
    int32_t currentLoopCount_;

    // states
    float attackPower_;

    // frag
    bool isInLoopWait_ = false;
    bool isHit_        = false;
    bool isFinish_     = false;

public:
    Vector3 GetCollisionPos() const override;
    const PlayerComboAttackData* GetComboAttackData() const { return comboAttackData_; }
    float GetAttackPower() const { return attackPower_; };
    int32_t GetCurrentLoopCount() const { return currentLoopCount_; }
    const bool& GetIsInLoopWait() const { return isInLoopWait_; }
    const bool& GetIsHit() const { return isHit_; }
    const bool& GetIsFinish() const { return isFinish_; }
    const WorldTransform* GetPlayerTransform() const { return baseTransform_; }
   const bool& GetHasHitEnemy() const { return hasHitEnemy_; }

    void SetAttackPower(float atkPower) { attackPower_ = atkPower; }
    void SetIsHit(const bool& is) { isHit_ = is; }
    void SetPlayerBaseTransform(const WorldTransform* playerBaseTransform);
    void SetParentTransform(WorldTransform* transform) override;
};