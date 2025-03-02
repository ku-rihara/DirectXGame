#pragma once

#include "3d/ViewProjection.h"
// function
#include "Easing.h"
// class
#include "BaseObject/BaseObject.h"
#include"CollisionBox/EnemyCollisionBox.h"
#include"Collider/AABBCollider.h"
#include"Behavior/BaseEnemyBehavior.h"
#include"Behavior/BaseEnemyMoveBehavior.h"
#include"utility/Particle/ParticleEmitter.h"
#include"Enemy/HPBar/EnemyHPBar.h"
#include"Effect/Effect.h"

#include"IsFindSprite/FindSprite.h"
#include"IsFindSprite/NotFindSprite.h"

class Player;
class GameCamera;
class BaseEnemy : public BaseObject,public AABBCollider {
public:
	struct Paramater {
		float chaseDistance;
		float chaseSpeed;
		float basePosY;
		float thrustRotateSpeed;
		float upperGravity;
		float upperFallSpeedLimit;
		float upperJumpPower;
		float archingbackValue;
		float archingbackRatio;
		float archingbackTime;
		float blowValue;
		float blowValueY;
		float blowTime;
		float blowRotateSpeed;
		float burstTime;
		float blowGravity;
	};
	enum class Type {
		NORMAL,
		STRONG,
	};
	struct ParticleEffect {
		std::string name;
		std::unique_ptr<ParticleEmitter>emitter;
	};

private:
	/// Particle
	std::array<ParticleEffect, 4>deathParticle_;
	std::array<ParticleEffect, 1>debriParticle_;
	std::array<ParticleEffect, 1>damageParticle;
	std::unique_ptr<ParticleEmitter>fallCrack_;
private:
	int deathSound_;
	int thurstSound_;
protected:

	Type type_;
	Paramater paramater_;

	/// other class
	Player* pPlayer_;
	GameCamera* pGameCamera_;
	std::unique_ptr<FindSprite>findSprite_;
	std::unique_ptr<NotFindSprite>notFindSprite_;
	std::unique_ptr<EnemyCollisionBox>collisionBox_;


	bool isdeath_;
	float hp_;
	float HPMax_;
	Vector2 hpbarSize_;
	std::unique_ptr<EnemyHPBar>hpbar_;
	float damageParm_;

	WorldTransform bodyTransform_;
	std::unique_ptr<Object3d>bodyObj_;

	/// behavior
	std::unique_ptr<BaseEnemyBehavior>damageBehavior_ = nullptr;
	std::unique_ptr<BaseEnemyMoveBehavior>moveBehavior_ = nullptr;
	
	std::list<std::unique_ptr<Effect>> effects_;
public:
	static Vector3 InitScale_;
public:
	BaseEnemy();

	///========================================================================================
	///  public method
	///========================================================================================

	/// 初期化、更新、描画
	virtual void Init(const Vector3& spownPos);
	virtual void Update();
	virtual void Draw(const ViewProjection& viewProjection);
	virtual void SpriteDraw(const ViewProjection& viewProjection);

	void DamageEmit();
	void ThrustEmit();
	void DeathEmit();
	void FallEffectUpdate();
	void FallEffectInit(const Vector3& pos);
	void RotateInit();

	/// jump
	void Jump(float& speed, const float& fallSpeedLimit,const float& gravity);
	void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump);
	
	Vector3 GetDirectionToTarget(const Vector3&target);

	void DamageForPar(const float& par);

	/// Hpバー処理
	virtual void DisplaySprite(const ViewProjection& viewProjection);

	/// Behavior
	void ChangeBehavior    (std::unique_ptr<BaseEnemyBehavior>behavior);
	void ChangeMoveBehavior(std::unique_ptr<BaseEnemyMoveBehavior>behavior);
	void BackToDamageRoot  ();
	void BehaviorChangeDeath();

	// ヘルパー関数: ParticleEffect を初期化する
	void InitParticleEffect(ParticleEffect& effect, const std::string& name, const std::string& modelName, const uint32_t& textureHandle, const int32_t& maxnum);

	/// ====================================================================
    /// Collision
    /// ====================================================================
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionStay ([[maybe_unused]] BaseCollider* other)override;
	Vector3 GetCollisionPos() const override;
	
	///========================================================================================
	///  getter method
	///========================================================================================
	bool           GetIsDeath()         const   { return isdeath_; }
	Type           GetType()            const   { return type_; }
	Paramater      GetParamater()       const   { return paramater_; }
	Player*        GetPlayer()                  { return pPlayer_; }
	GameCamera*    GetGameCamera()              { return pGameCamera_; }
	FindSprite*    GetFindSprite()              { return findSprite_.get(); }
	NotFindSprite* GetNotFindSprite()           { return notFindSprite_.get(); }
	///========================================================================================
	///  setter method
	///========================================================================================
	void SetPlayer     (Player* plyaer);
	void SetGameCamera (GameCamera* gamecamera);
	void SetParamater  (const Type&type,const Paramater& paramater);
	void SetBodyRotateX(const float& r) { bodyTransform_.rotation_.x = r; }
	void SetBodyColor(const Vector4& color);
	void SetIsDeath(const bool& is) { isdeath_ = is; }

private:
	bool IsInView(const ViewProjection& viewProjection) const;
	
};
