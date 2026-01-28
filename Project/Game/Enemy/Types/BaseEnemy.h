#pragma once

// 3D
#include "3d/ViewProjection.h"
// BaseObject
#include "BaseObject/BaseObject.h"
// 3D
#include "3D/AnimationObject3D/Object3DAnimation.h"
// Behavior
#include "../Behavior/ActionBehavior/BaseEnemyBehavior.h"
#include "../Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
// Collision
#include "../CollisionBox/EnemyAttackCollisionBox.h"
#include "Collider/AABBCollider.h"
#include "Enemy/Effects/EnemyEffects.h"
#include "Enemy/HPBar/EnemyHPBar.h"
// std
#include <cstdint>
#include <memory>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class PlayerAttackCollisionBox;

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public BaseObject, public KetaEngine::AABBCollider {
public:
    enum class Type {
        NORMAL,
        STRONG,
        COUNT,
    };

    struct AttackParam {
        float startTime;
        float attackValue;
        Vector3 collisionSize;
        Vector3 collisionOffset;
        float adaptTime;
    };

    struct Parameter {
        AttackParam attackParam;
        Vector3 baseScale_;
        Vector2 hpBarPosOffset;
        float chaseDistance;
        float chaseSpeed;
        float basePosY;
        float burstTime;
        float avoidanceRadius;
        float maxChaseTime;
        float chaseResetTime;
        float chaseLimitDistance;
        // attackParam
        float attackStartDistance;
        float attackCooldownTime;
        float attackAnticipationTime;
        // death Param
        float deathBlowValue;
        float deathBlowValueY;
        float deathGravity;
        float deathRotateSpeed;
        float deathBurstTime;
    };

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
    /// 攻撃予備動作
    /// </summary>
    virtual void AttackAnticipation() = 0;

    /// <summary>
    /// 攻撃予備動作が完了したか
    /// </summary>
    virtual bool IsAttackAnticipationFinished() = 0;

    /// <summary>
    /// 攻撃開始処理
    /// </summary>
    virtual void AttackStart() = 0;

    /// <summary>
    /// 攻撃更新処理
    /// </summary>
    virtual void AttackUpdate() = 0;

    /// <summary>
    /// 攻撃が完了したか
    /// </summary>
    virtual bool IsAttackFinished() = 0;

    /// <summary>
    /// 攻撃終了処理
    /// </summary>
    virtual void AttackFinish() = 0;

    /// <summary>
    /// スプライトUIの表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    virtual void DisplaySprite(const KetaEngine::ViewProjection& viewProjection);

    void ThrustRenditionInit(); //< 突き飛ばし演出初期化
    void DeathRenditionInit(); //< 死亡演出初期化
    void ScaleReset(); //< スケールリセット
    void RotateInit(); //< 回転初期化

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
    void Fall(float& speed, float fallSpeedLimit, float gravity, const bool& isJump);

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

    // behavior変更
    void ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior);
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);

    void BackToDamageRoot(); //< ダメージルートに戻る

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

    // ダメージクールタイムの更新
    void DamageCollingUpdate(float deltaTime);

    // ダメージリアクション変更処理
    void ChangeDamageReactionByPlayerAttack(PlayerAttackCollisionBox* attackController);

private:
    void MoveToLimit();

private:
    // 敵グループのID
    int32_t groupId_;

    // HPバーのサイズ
    Vector2 hpBarSize_;

    // other class
    Player* pPlayer_;
    Combo* pCombo_;
    GameCamera* pGameCamera_;
    EnemyManager* pEnemyManager_;
  

    // flags
    bool isDeathPending_ = false;
    bool isDamageColling_;
    bool isDeath_;
    bool isCollisionRope_;

    // damageInfo
    float damageCollTime_;
    std::string lastReceivedAttackName_;

protected:
 
    std::unique_ptr<KetaEngine::Object3d> obj3d_;
    std::unique_ptr<KetaEngine::Object3DAnimation> objAnimation_;

    std::unique_ptr<EnemyAttackCollisionBox> attackCollisionBox_;
    std::unique_ptr<EnemyHPBar> hpBar_;
    std::unique_ptr<EnemyEffects> enemyEffects_;

    // flags
    bool isReturningFromAttack_;

    /// behavior
    std::unique_ptr<BaseEnemyDamageReaction> damageBehavior_ = nullptr;
    std::unique_ptr<BaseEnemyBehavior> moveBehavior_         = nullptr;

    // パラメータ
    Parameter parameter_;

    // Type
    Type type_;

    // HP
    float hp_;
    float HPMax_;

public:
    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    const bool& GetIsDeath() const { return isDeath_; }
    const bool& GetIsDeathPending() const { return isDeathPending_; }
    const Type& GetType() const { return type_; }
    const Parameter& GetParameter() const { return parameter_; }
    int32_t GetGroupId() const { return groupId_; }
    float GetHP() const { return hp_; }
    Vector3 GetBodyRotation() const { return obj3d_->transform_.rotation_; }
    Player* GetPlayer() const { return pPlayer_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    BaseEnemyDamageReaction* GetDamageReactionBehavior() const { return damageBehavior_.get(); }
    EnemyManager* GetManager() const { return pEnemyManager_; }
    EnemyEffects* GetEnemyEffects() const { return enemyEffects_.get(); }
    EnemyAttackCollisionBox* GetAttackCollisionBox() const { return attackCollisionBox_.get(); }
    KetaEngine::Object3d* GetObject3D() const { return obj3d_.get(); }
    bool GetIsReturningFromAttack() const { return isReturningFromAttack_; }

    /// ========================================================================================
    ///  setter method
    /// ========================================================================================
    void SetPlayer(Player* plyaer);
    void SetGameCamera(GameCamera* gamecamera);
    void SetManager(EnemyManager* manager);
    void SetCombo(Combo* combo);
    void SetParameter(const Type& type, const Parameter& paramater);
    void SetBodyRotateX(float rotate) { obj3d_->transform_.rotation_.x = rotate; }
    void SetBodyColor(const Vector4& color);
    void SetIsDeath(const bool& is) { isDeath_ = is; }
    void SetGroupId(const int& groupId) { groupId_ = groupId; }
    void SetIsDeathPending(const bool& is) { isDeathPending_ = is; }
    void SetWorldPositionY(float PosY) { baseTransform_.translation_.y = PosY; }
    void SetIsReturningFromAttack(bool is) { isReturningFromAttack_ = is; }
};
