#pragma once

#include "3d/ViewProjection.h"
// function

// class
#include "../Behavior/DamageReactionBehavior/BaseEnemyDamageReaction.h"
#include "../Behavior/NormalBehavior/BaseEnemyBehavior.h"
#include "BaseObject/BaseObject.h"
#include "Collider/AABBCollider.h"
#include "CollisionBox/EnemyCollisionBox.h"
#include "Enemy/HPBar/EnemyHPBar.h"

#include "../SearchingSprite/FindSprite.h"
#include "../SearchingSprite/NotFindSprite.h"
#include <cstdint>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class AttackEffect;

/// <summary>
/// 敵の基底クラス
/// </summary>
class BaseEnemy : public BaseObject, public AABBCollider {
public:
    enum class Type {
        NORMAL,
        STRONG,
        COUNT,
    };

    struct Parameter {
        Vector3 initScale_;
        Vector2 hpBarPosOffset;
        float chaseDistance;
        float chaseSpeed;
        float basePosY;
        float thrustRotateSpeed;
        float upperGravity;
        float upperFallSpeedLimit;
        float upperJumpPower;
        float archingBackValue;
        float archingBackRate;
        float archingBackTime;
        float blowValue;
        float blowValueY;
        float blowTime;
        float blowRotateSpeed;
        float burstTime;
        float blowGravity;
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

    /// <summary>
    /// スプライトUIの表示
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    virtual void DisplaySprite(const ViewProjection& viewProjection);

    virtual void SpawnRenditionInit() = 0; //< スポーン演出初期化

    void DamageRenditionInit(); //< ダメージ演出初期化
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
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity);

    /// <summary>
    /// 落下処理
    /// </summary>
    /// <param name="speed">速度</param>
    /// <param name="fallSpeedLimit">落下速度制限</param>
    /// <param name="gravity">重力</param>
    /// <param name="isJump">ジャンプフラグ</param>
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump);

    /// <summary>
    /// ターゲットへの方向取得
    /// </summary>
    /// <param name="target">ターゲット位置</param>
    Vector3 GetDirectionToTarget(const Vector3& target);

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damageValue">ダメージ量</param>
    void TakeDamage(const float& damageValue);

    /// <summary>
    /// ビヘイビア変更
    /// </summary>
    /// <param name="behavior">新しいビヘイビア</param>
    void ChangeDamageReactionBehavior(std::unique_ptr<BaseEnemyDamageReaction> behavior);

    /// <summary>
    /// 移動ビヘイビア変更
    /// </summary>
    /// <param name="behavior">新しい移動ビヘイビア</param>
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);

    void BackToDamageRoot(); //< ダメージルートに戻る
    void BehaviorChangeDeath(); //< 死亡ビヘイビアに変更

    /// ====================================================================
    /// Collision
    /// ====================================================================

    /// <summary>
    /// 衝突開始時コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;

    /// <summary>
    /// 衝突中コールバック
    /// </summary>
    /// <param name="other">衝突相手のコライダー</param>
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;

    Vector3 GetCollisionPos() const override;

private:
    /// <summary>
    /// 視野内判定
    /// </summary>
    /// <param name="viewProjection">ビュープロジェクション</param>
    bool IsInView(const ViewProjection& viewProjection) const;

private:
    int deathSound_;
    int thrustSound_;
    int32_t groupId_;
protected:
    // structure
    Type type_;
    Parameter parameter_;

    /// other class
    Player* pPlayer_;
    Combo* pCombo_;
    GameCamera* pGameCamera_;
    EnemyManager* pEnemyManager_;
    AttackEffect* pAttackEffect_;

    std::unique_ptr<FindSprite> findSprite_;
    std::unique_ptr<NotFindSprite> notFindSprite_;
    std::unique_ptr<EnemyCollisionBox> collisionBox_;
    std::unique_ptr<EnemyHPBar> hpBar_;

    // parameter
    bool isDeath_;
    float hp_;
    float HPMax_;
    Vector2 hpBarSize_;

    /// behavior
    std::unique_ptr<BaseEnemyDamageReaction> damageBehavior_ = nullptr;
    std::unique_ptr<BaseEnemyBehavior> moveBehavior_     = nullptr;

public:
    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    const bool& GetIsDeath() const { return isDeath_; }
    const Type& GetType() const { return type_; }
    const Parameter& GetParameter() const { return parameter_; }
    const int32_t& GetGroupId() const { return groupId_; }
    Vector3 GetBodyRotation() const {return obj3d_->transform_.rotation_;}
    Player* GetPlayer() const { return pPlayer_; }
    GameCamera* GetGameCamera() const { return pGameCamera_; }
    BaseEnemyDamageReaction* GetDamageReactionBehavior() const { return damageBehavior_.get(); }
    FindSprite* GetFindSprite() const { return findSprite_.get(); }
    NotFindSprite* GetNotFindSprite() const { return notFindSprite_.get(); }
    EnemyManager* GetManager() const { return pEnemyManager_; }
    /// ========================================================================================
    ///  setter method
    /// ========================================================================================
    void SetPlayer(Player* plyaer);
    void SetGameCamera(GameCamera* gamecamera);
    void SetManager(EnemyManager* manager);
    void SetCombo(Combo* combo);
    void SetAttackEffect(AttackEffect* attackEffect);
    void SetParameter(const Type& type, const Parameter& paramater);
    void SetBodyRotateX(const float& r) { obj3d_->transform_.rotation_.x = r; }
    void SetBodyColor(const Vector4& color);
    void SetIsDeath(const bool& is) { isDeath_ = is; }
    void SetGroupId(const int& groupId) { groupId_ = groupId; }
    void SetWorldPositionY(const float& y) {baseTransform_.translation_.y = y; }
};