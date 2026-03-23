#pragma once

// BaseCollisionBox
#include "Collider/SphereCollider.h"
// Combo
#include "Combo/Combo.h"
// Parameter
#include "Editor/ParameterEditor/GlobalParameter.h"
// std
#include <array>
#include <string>

class PlayerComboAttackData;
/// <summary>
/// プレイヤー攻撃の制御クラス
/// </summary>
class PlayerAttackCollider : public KetaEngine::SphereCollider {

public:
    PlayerAttackCollider()           = default;
    ~PlayerAttackCollider() override = default;

    // 初期化、更新、描画
    void Init() override;
    void Update();
    void TimerUpdate(float timeSpeed);

    void UpdateOffset();

    // 攻撃情報取得
    void AttackStart(const PlayerComboAttackData* comboAttackData);

    // コリジョンコールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

private:
    void AdaptCollision();
    void LoopWaiting(float timeSpeed);
    void LoopStart();

private:
    const KetaEngine::WorldTransform* baseTransform_ = nullptr;
    const PlayerComboAttackData* comboAttackData_    = nullptr;

    KetaEngine::WorldTransform transform_;
    Vector3 offset_;
    float sphereRad_;

    // ヒットした敵の座標（ポインタは持たない）
    Vector3 hitTargetPos_  = {};
    bool    hasHitTarget_  = false;

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

    bool isAbleCollision_ = false;

    // 実際にダメージが入った回数（敵のダメージインターバル通過時にインクリメント）
    int32_t damageHitCount_ = 0;

public:
    Vector3 GetCollisionPos() const override;
    const PlayerComboAttackData* GetComboAttackData() const { return comboAttackData_; }
    float GetAttackPower() const { return attackPower_; };
    int32_t GetCurrentLoopCount() const { return currentLoopCount_; }
    const bool& GetIsInLoopWait() const { return isInLoopWait_; }
    const bool& GetIsHit() const { return isHit_; }

    // ダメージが確定した際に敵側から呼ぶ
    void NotifyDamageHit() { ++damageHitCount_; }
    int32_t GetDamageHitCount() const { return damageHitCount_; }
    const bool& GetIsFinish() const { return isFinish_; }
    const KetaEngine::WorldTransform* GetPlayerTransform() const { return baseTransform_; }
    const bool& GetHasHitEnemy() const { return hasHitEnemy_; }
    bool            GetHasHitTarget() const  { return hasHitTarget_; }
    const Vector3&  GetHitTargetPos() const  { return hitTargetPos_; }

    void SetIsAbleCollision(const bool& is) { isAbleCollision_ = is; }
    void SetAttackPower(float atkPower) { attackPower_ = atkPower; }
    void SetIsHit(const bool& is) { isHit_ = is; }
    void SetPlayerBaseTransform(const KetaEngine::WorldTransform* playerBaseTransform);
    void SetSphereRad(float radius) { sphereRad_ = radius; }
    void SetPosition(const Vector3& position) { transform_.translation_ = position; }
   
};