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

class Player;
class BaseEnemy : public BaseObject,public AABBCollider {
protected:

	/// other class
	Player* pPlayer_;
	bool isdeath_;
	float hp_;
	float HPMax_;
	Vector2 hpbarSize_;
	std::unique_ptr<EnemyHPBar>hpbar_;
	float damageParm_;

	Easing spawnEasing_;

	/// behavior
	std::unique_ptr<BaseEnemyBehaivor>behavior_ = nullptr;

	/// エミッター
	std::unique_ptr<ParticleEmitter>emitter_;

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
	
	Vector3 GetDirectionToTarget(const Vector3&target);

	void DamageForPar(const float& par);

	/// Hpバー処理
	virtual void DisplayHpBar(const ViewProjection& viewProjection);

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
	///========================================================================================
	///  setter method
	///========================================================================================
	void SetPlayer(Player* plyaer);
	

private:
	bool IsInView(const ViewProjection& viewProjection) const;
	
};
