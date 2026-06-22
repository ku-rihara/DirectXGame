#pragma once

// 3D
#include "3D/ViewProjection.h"
// BaseObject
#include "BaseObject/BaseObject.h"
// Behavior
#include "../Behavior/ActionBehavior/BaseEnemyBehavior.h"
#include "../Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
// BehaviorController
#include "../BehaviorController/EnemyBehaviorController.h"
// Collision
#include "../CollisionBox/EnemyAttackCollisionBox.h"
#include "Collider/SphereCollider.h"
#include "Enemy/Effects/EnemyEffects.h"
#include "Enemy/UIs/EnemyUIs.h"
#include "Enemy/Component/EnemyAnimator.h"
#include "Enemy/Component/EnemyBaseInfo.h"

// std
#include <cstdint>
#include <functional>
#include <memory>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class KillCounter;
class PlayerAttackCollider;
class EnemyHPBarColorConfig;
class BaseEnemyBehavior;
class EnemyInitializer;

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public BaseObject, public KetaEngine::SphereCollider {
    friend class EnemyInitializer;

public:
    using Type                   = EnemyBaseInfo::Type;
    using Parameter              = EnemyBaseInfo::Parameter;
    using AnimationType          = EnemyAnimator::AnimationType;
    using ChaseAnimationState    = EnemyAnimator::ChaseAnimationState;
    using DamageReactionAnimInfo = EnemyAnimator::DamageReactionAnimInfo;

public:
    BaseEnemy()          = default;
    virtual ~BaseEnemy() = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    // 初期化、更新
    virtual void Init(const Vector3& spownPos);
    virtual void Update();

    // 振る舞い個別処理
    virtual void SpawnRenditionInit() = 0;

    /// <summary>
    /// スポーン後の行動を生成
    /// </summary>
    virtual std::unique_ptr<BaseEnemyBehavior> CreatePostSpawnBehavior();

    void ThrustRenditionInit(); //< 突き飛ばし演出初期化
    void DeathRenditionInit(); //< 死亡演出初期化
    void ScaleReset(); //< スケールリセット

    /// <summary>
    /// プールへ返却する前に呼ぶクリーンアップ
    /// </summary>
    virtual void PrepareForPool();

    /// <summary>
    /// スプライトUIの表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void DisplaySprite(const KetaEngine::ViewProjection& viewProjection, float distanceToPlayer, bool isOccluded = false);

    /// <summary>
    /// ジャンプ処理
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    void Jump(float& speed, float fallSpeedLimit, float gravity);

    /// <summary>
    /// 落下処理
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプフラグ</param>
    void Fall(float& speed, float fallSpeedLimit, float gravity, bool isJump);

    /// <summary>
    /// ターゲットへの方向取得
    /// </summary>
    /// <param name="target">ターゲット位置</param>
    Vector3 GetDirectionToTarget(const Vector3& target);

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damageValue">ダメージ量</param>
    void TakeDamage(float damageValue);

    // ヒットクールタイム開始
    void StartDamageColling(float collingTime, const std::string& reactiveAttackName);

    // ダメージを受けたときのコールバックを設定
    void SetOnDamageTakenCallback(std::function<void()> cb) { onDamageTaken_ = std::move(cb); }

    virtual void AdjustParam() {}

    // behavior変更
    void ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior);
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);
    virtual void BackToDamageRoot();

    /// <summary>
    /// プレイヤーの方向を向く
    /// </summary>
    void DirectionToPlayer(bool isOpposite = false);
    /// <summary>
    /// プレイヤーとの距離を計算
    /// </summary>
    /// <returns></returns>
    float CalcDistanceToPlayer();
    /// ====================================================================
    /// Collision
    /// ====================================================================

    // 衝突コールバック
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    Vector3 GetCollisionPos() const override;


private:
    /// <summary>
    /// 視野内判定
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    bool IsInView(const KetaEngine::ViewProjection& viewProjection) const;

private:
    void MoveToLimit();

private:
    // 敵グループのID
    int32_t groupId_;

    // HPバーのサイズ
    Vector2 hpBarSize_;

    EnemyBaseInfo baseInfo_;
    EnemyAnimator animator_;

    // ダメージコールバック
    std::function<void()> onDamageTaken_;

protected:
    // flags
    bool isDeath_          = false;
    bool isDeathPending_   = false;
    bool isCollisionRope_  = false;
    bool isInAnticipation_ = false;
    bool isAttacking_      = false;

    void ResetDamageCooling() { behaviorCtrl_.ResetDamageCooling(); }

protected:
    std::unique_ptr<EnemyAttackCollisionBox> attackCollisionBox_;
    std::unique_ptr<EnemyUIs> enemyUIs_;
    std::unique_ptr<EnemyEffects> enemyEffects_;

    /// ビヘイビア管理
    EnemyBehaviorController behaviorCtrl_;

    // ボディカラー更新用
    EnemyHPBarColorConfig* colorConfig_ = nullptr;

    float hp_;
    float HPMax_;

public:
    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    bool GetIsDeath() const { return isDeath_; }
    bool GetIsDeathPending() const { return isDeathPending_; }
    bool IsInDeathBehavior() const;
    int32_t GetGroupId() const { return groupId_; }
    float GetHP() const { return hp_; }
    float GetBaseRotationY() const { return baseTransform_.rotation_.y; }
    BaseEnemyDamageReaction* GetDamageReactionBehavior() const { return behaviorCtrl_.GetDamageBehavior(); }
    EnemyBaseInfo* GetBaseInfo() { return &baseInfo_; }
    const EnemyBaseInfo* GetBaseInfo() const { return &baseInfo_; }
    EnemyAnimator* GetAnimator() { return &animator_; }
    const EnemyAnimator* GetAnimator() const { return &animator_; }
    EnemyEffects* GetEnemyEffects() const { return enemyEffects_.get(); }
    EnemyAttackCollisionBox* GetAttackCollisionBox() const { return attackCollisionBox_.get(); }
    bool IsInAnticipation() const { return isInAnticipation_; }
    bool IsAttacking() const { return isAttacking_; }

    /// ========================================================================================
    ///  setter method
    /// ========================================================================================
    void RefreshCollision();
    void SetIsDeath(bool is);
    void SetGroupId(int groupId) { groupId_ = groupId; }
    void SetIsDeathPending(bool is) { isDeathPending_ = is; }
    void SetWorldPositionY(float PosY) { baseTransform_.translation_.y = PosY; }
    void SetHPBarColorConfig(EnemyHPBarColorConfig* config) {
        colorConfig_ = config;
        if (enemyUIs_) { enemyUIs_->SetColorConfig(config); }
    }
    void SetGroupIconIndex(int32_t index) {
        if (enemyUIs_) { enemyUIs_->SetGroupIndex(index); }
    }
    void OnSpawnCompleted() { if (enemyUIs_) { enemyUIs_->OnSpawned(); } }
    void SetIsInAnticipation(bool value) { isInAnticipation_ = value; }
    void SetIsAttacking(bool value) { isAttacking_ = value; }

};