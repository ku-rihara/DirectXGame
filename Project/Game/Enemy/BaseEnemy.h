#pragma once

#include "3d/ViewProjection.h"
// function
#include "EasingFunction.h"
// class
#include "BaseObject/BaseObject.h"
#include "Behavior/BaseEnemyBehavior.h"
#include "Behavior/BaseEnemyMoveBehavior.h"
#include "Collider/AABBCollider.h"
#include "CollisionBox/EnemyCollisionBox.h"
#include "Enemy/HPBar/EnemyHPBar.h"

#include "SearchingSprite/FindSprite.h"
#include "SearchingSprite/NotFindSprite.h"
#include <cstdint>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class BaseEnemy : public BaseObject, public AABBCollider {
public:
    enum class Type {
        NORMAL,
        STRONG,
        COUNT,
    };

    struct Parameter {
        Vector3 initScale_;
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

private:
    int deathSound_;
    int thurstSound_;
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
    std::unique_ptr<BaseEnemyBehavior> damageBehavior_   = nullptr;
    std::unique_ptr<BaseEnemyMoveBehavior> moveBehavior_ = nullptr;

public:
    BaseEnemy()          = default;
    virtual ~BaseEnemy() = default;

    ///========================================================================================
    ///  public method
    ///========================================================================================

    /// 初期化、更新、描画
    virtual void Init(const Vector3& spownPos);
    virtual void Update();
    virtual void SpriteDraw(const ViewProjection& viewProjection);
    virtual void DisplaySprite(const ViewProjection& viewProjection);

    /// Rendition Init
    void DamageRenditionInit();
    void ThrustRenditionInit();
    void DeathRenditionInit();
    virtual void SpawnRenditionInit() = 0;

    void ScaleReset();
    void RotateInit();

    /// jump
    void Jump(float& speed, const float& fallSpeedLimit, const float& gravity);
    void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump);

    Vector3 GetDirectionToTarget(const Vector3& target);

    void TakeDamage(const float& par);


    /// Behavior
    void ChangeBehavior(std::unique_ptr<BaseEnemyBehavior> behavior);
    void ChangeMoveBehavior(std::unique_ptr<BaseEnemyMoveBehavior> behavior);
    void BackToDamageRoot();
    void BehaviorChangeDeath();

    /// ====================================================================
    /// Collision
    /// ====================================================================
    void OnCollisionEnter([[maybe_unused]] BaseCollider* other) override;
    void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;
    Vector3 GetCollisionPos() const override;

    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    bool GetIsDeath() const { return isDeath_; }
    Type GetType() const { return type_; }
    Parameter GetParameter() const { return parameter_; }
    Player* GetPlayer() { return pPlayer_; }
    GameCamera* GetGameCamera() { return pGameCamera_; }
    BaseEnemyBehavior* GetDamageBehavior() const { return damageBehavior_.get(); }
    FindSprite* GetFindSprite() { return findSprite_.get(); }
    NotFindSprite* GetNotFindSprite() { return notFindSprite_.get(); }
    int32_t GetGroupId() const { return groupId_; }
    /// ========================================================================================
    ///  setter method
    /// ========================================================================================
    void SetPlayer(Player* plyaer);
    void SetGameCamera(GameCamera* gamecamera);
    void SetManager(EnemyManager* manager);
    void SetCombo(Combo* combo);
    void SetParameter(const Type& type, const Parameter& paramater);
    void SetBodyRotateX(const float& r) { obj3d_->transform_.rotation_.x = r; }
    void SetBodyColor(const Vector4& color);
    void SetIsDeath(const bool& is) { isDeath_ = is; }
    void SetGroupId(int groupId) { groupId_ = groupId; }

private:
    bool IsInView(const ViewProjection& viewProjection) const;
};
