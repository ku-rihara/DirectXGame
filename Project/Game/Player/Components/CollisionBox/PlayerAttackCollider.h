#pragma once

// BaseCollisionBox
#include "Collider/SphereCollider.h"
// Timeline
#include "Player/ComboCreator/Timeline/PlayerComboAttackTimelinePhase.h"
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
    void AttackStart(const PlayerComboAttackData* comboAttackData, AttackTimelinePhase phase = AttackTimelinePhase::MAIN);

    // コリジョンコールバック
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 新しい攻撃に備えて状態をリセット
    /// </summary>
    void PrepareForNewAttack();

    // ダメージがヒットしたことを通知
    void NotifyDamageHit() { ++damageHitCount_; }

private:
    void AdaptCollision();
    void LoopWaiting(float timeSpeed);
    void LoopStart();

private:

    // 攻撃データ
    const PlayerComboAttackData* comboAttackData_ = nullptr;

    // WorldTransformと攻撃コリジョンのオフセット
    KetaEngine::WorldTransform transform_;
    Vector3 offset_;

    // ヒットした敵の座標
    Vector3 hitTargetPos_ = {};

    // times
    float adaptTimer_;
    float loopWaitTimer_;

    // コリジョンOn/OFFのループ回数
    int32_t currentLoopCount_;

    // 攻撃力とサイズ
    float attackPower_;
    float sphereRad_;

    // 状態フラグ
    bool isInLoopWait_    = false;
    bool isHit_           = false;
    bool isFinish_        = false;
    bool isAbleCollision_ = false;
    bool hasHitTarget_    = false;

    // 攻撃のフェーズ
    AttackTimelinePhase phase_ = AttackTimelinePhase::MAIN;

    // 実際にダメージが入った回数
    int32_t damageHitCount_ = 0;

public:
    /// ===================================================
    /// Getter Method
    /// ===================================================
    Vector3 GetCollisionPos() const override;
    const PlayerComboAttackData* GetComboAttackData() const { return comboAttackData_; }
    float GetAttackPower() const { return attackPower_; };
    int32_t GetCurrentLoopCount() const { return currentLoopCount_; }
    bool GetIsHit() const { return isHit_; }
    int32_t GetDamageHitCount() const { return damageHitCount_; }
    bool GetIsFinish() const { return isFinish_; }
    const KetaEngine::WorldTransform* GetPlayerTransform() const { return transform_.parent_; }
    bool GetHasHitTarget() const { return hasHitTarget_; }
    const Vector3& GetHitTargetPos() const { return hitTargetPos_; }

    /// ===================================================
    /// Setter Method
    /// ===================================================
    void SetIsAbleCollision(bool is) { isAbleCollision_ = is; }
    void SetAttackPower(float atkPower) { attackPower_ = atkPower; }
    void SetPlayerBaseTransform(const KetaEngine::WorldTransform* playerBaseTransform);
    void SetSphereRad(float radius) { sphereRad_ = radius; }
    void SetPosition(const Vector3& position) { transform_.translation_ = position; }
};