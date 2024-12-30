#pragma once

#include "3d/ViewProjection.h"
// function
#include "Easing.h"
// class
#include "BaseObject/BaseObject.h"
#include"Collider/AABBCollider.h"
#include"Behavior/BaseEnemyBehavior.h"

class Player;
class BaseEnemy : public BaseObject,public AABBCollider {
protected:

	/// other class
	Player* pPlayer_;
	bool isdeath_;

	/// behavior
	std::unique_ptr<BaseEnemyBehaivor>behavior_ = nullptr;

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
	
	Vector3 GetDirectionToPlayer();

	/// Hpバー処理
	virtual void DisplayHpBar(const ViewProjection& viewProjection);

	void ChangeBehavior(std::unique_ptr<BaseEnemyBehaivor>behavior);


	// 当たり判定
	void OnCollisionEnter([[maybe_unused]] BaseCollider* other)override;
	
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
