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
#include"Field/SideRope/SideRopeReboundSystem.h"

#include "../SearchingSprite/FindSprite.h"
#include "../SearchingSprite/NotFindSprite.h"
#include <cstdint>

class Player;
class GameCamera;
class EnemyManager;
class Combo;
class AttackEffect;
class PlayerCollisionInfo;
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

     struct Parameter {
        Vector3 initScale_;
        Vector2 hpBarPosOffset;
        float chaseDistance;
        float chaseSpeed;
        float basePosY;
        float burstTime;
        float avoidanceRadius; 
        float maxChaseTime;    
        float chaseResetTime; 
        float chaseLimitDistance;

        float deathBlowValue; 
        float deathBlowValueY;
        float deathGravity;   
        float deathRotateSpeed;
        float deathBurstTime;  

        float gravity;
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
    virtual void DisplaySprite(const KetaEngine::ViewProjection& viewProjection);

    virtual void SpawnRenditionInit() = 0; //< スポーン演出初期化

    void DamageRenditionInit(); //< ダメージ演出初期化
    void ThrustRenditionInit(); //< 突き飛ばし演出初期化
    void DeathRenditionInit();  //< 死亡演出初期化
    void ScaleReset();          //< スケールリセット
    void RotateInit();          //< 回転初期化

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
    void StartDamageColling(float collingTime, const std::string&reactiveAttackName);
  
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

    //　ダメージクールタイムの更新
    void DamageCollingUpdate(float deltaTime);

   
    // ダメージリアクション変更処理
    void ChangeDamageReactionByPlayerAttack(PlayerCollisionInfo* attackController);

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
    float hp_;
    float HPMax_;
    Vector2 hpBarSize_;

    // frags
    bool isDeathPending_ = false;
    bool isDamageColling_;
    bool isDeath_;
    bool isCollisionRope_;

    // hitParam
    float damageCollTime_;
    std::string lastReceivedAttackName_;

    /// behavior
    std::unique_ptr<BaseEnemyDamageReaction> damageBehavior_ = nullptr;
    std::unique_ptr<BaseEnemyBehavior> moveBehavior_         = nullptr;

    // ropeReboundSystem
    SideRopeReboundSystem ropeReboundSystem_;

public:
    /// ========================================================================================
    ///  getter method
    /// ========================================================================================
    const bool& GetIsDeath() const { return isDeath_; }
    const bool& GetIsDeathPending() const { return isDeathPending_; }
    const Type& GetType() const { return type_; }
    const Parameter& GetParameter() const { return parameter_; }
    SideRopeReboundSystem& GetSideRopeReboundSystem() { return ropeReboundSystem_; }
    int32_t GetGroupId() const { return groupId_; }
    float GetHP() const { return hp_; } 
    Vector3 GetBodyRotation() const { return obj3d_->transform_.rotation_; }
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
    void SetBodyRotateX(float r) { obj3d_->transform_.rotation_.x = r; }
    void SetBodyColor(const Vector4& color);
    void SetIsDeath(const bool& is) { isDeath_ = is; }
    void SetGroupId(const int& groupId) { groupId_ = groupId; }
    void SetIsDeathPending(const bool& is) { isDeathPending_ = is; } 
    void SetWorldPositionY(float y) { baseTransform_.translation_.y = y; }
};