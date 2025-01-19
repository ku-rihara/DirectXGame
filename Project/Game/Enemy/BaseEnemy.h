#pragma once

#include "3d/ViewProjection.h"
// function
#include "Easing.h"
// class
#include "BaseObject/BaseObject.h"
#include"Collider/AABBCollider.h"
#include"Behavior/BaseEnemyBehavior.h"
#include"utility/Particle/ParticleEmitter.h"
#include"Enemy/HPBar/EnemyHPBar.h"
#include"Effect/Effect.h"

#include"IsFindSprite/FindSprite.h"
#include"IsFindSprite/NotFindSprite.h"

class Player;
class GameCamera;
class BaseEnemy : public BaseObject,public AABBCollider {
protected:

	/// other class
	Player* pPlayer_;
	GameCamera* pGameCamera_;
	std::unique_ptr<FindSprite>findSprite_;
	std::unique_ptr<NotFindSprite>notFindSprite_;


	bool isdeath_;
	float hp_;
	float HPMax_;
	Vector2 hpbarSize_;
	std::unique_ptr<EnemyHPBar>hpbar_;
	float damageParm_;

	Easing spawnEasing_;

	std::string damageName_;
	std::unique_ptr<ParticleEmitter>damageEmitter_;

	std::string thrustName_;
	std::unique_ptr<ParticleEmitter>thrustEmit_;

	/// behavior
	std::unique_ptr<BaseEnemyBehaivor>behavior_ = nullptr;

	/// エミッター
	std::unique_ptr<ParticleEmitter>emitter_;
	std::list<std::unique_ptr<Effect>> effects_;
public:
	static float InitY_;
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
	void FallEffectUpdate();
	void FallEffectInit(const Vector3& pos);
	
	Vector3 GetDirectionToTarget(const Vector3&target);

	void DamageForPar(const float& par);

	/// Hpバー処理
	virtual void DisplaySprite(const ViewProjection& viewProjection);

	void ChangeBehavior(std::unique_ptr<BaseEnemyBehaivor>behavior);


	// 当たり判定
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	void OnCollisionStay([[maybe_unused]] BaseCollider* other)override;

	Vector3 GetCollisionPos() const override;
	///========================================================================================
	///  getter method
	///========================================================================================
	bool GetIsDeath()const { return isdeath_; }
	Player* GetPlayer() { return pPlayer_; }
	GameCamera* GetGameCamera() { return pGameCamera_; }
	FindSprite* GetFindSprite() { return findSprite_.get(); }
	NotFindSprite* GetNotFindSprite() { return notFindSprite_.get(); }
	///========================================================================================
	///  setter method
	///========================================================================================
	void SetPlayer(Player* plyaer);
	void SetGameCamera(GameCamera* gamecamera);


private:
	bool IsInView(const ViewProjection& viewProjection) const;
	
};
