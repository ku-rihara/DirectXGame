#pragma once

#include "3d/ViewProjection.h"
#include <memory>

//class
#include"BaseObject/BaseObject.h"
#include"Collider/AABBCollider.h"
/// behavior
#include"PlayerBehavior/BasePlayerBehavior.h"

/// <summary>
/// プレイヤークラス
/// </summary>
class LockOn;
class Player : public BaseObject{

	
private:
	/// ===================================================
	///private variaus
	/// ===================================================
	
	/// other class
	LockOn* pLockOn_;
	
	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	/// 攻撃フラグ
	bool isAttack_;

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;

	/// 目標角度
	float objectiveAngle_;
	/// 速度
	Vector3 velocity_;
	Vector3 prePos_;
	
	/// jump
	float jumpSpeed_;	/// ジャンプスピード
	float muzzelJumpSpeed_;	/// ジャンプ
	const float jumpLimit_ = -1.2f;

	// fall
	float fallSpeed_;

public:
	static float InitY_;
	Player();

	/// ===================================================
	///public method
	/// ===================================================

	//初期化、更新、
	void Init() override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	/// 移動
	void Move(const float& speed);
	bool GetIsMoving();
	Vector3 GetInputVelocity();
	void MoveToLimit();

	
	void Jump(float& speed);/// ジャンプ
	void Fall();
	void ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior);
	
	void Debug();

	void SetLockOn(LockOn* lockon);
	
	/// collision

	/// ダメージ
	void DamageRendition();
	void TakeDamage();
	
	/// ===================================================
	/// getter
	/// ===================================================
	const bool& GetIsAttack()const { return isAttack_; }


	float GetMuzzulJumpSpeed()const { return muzzelJumpSpeed_; }
	/// ===================================================
	/// setter
	/// ===================================================
	/*void SetJumpSpeed(float speed) { muzzelJumpSpeed_ = speed; }*/
	void SetIsAttack(bool is) { isAttack_ = is; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }


};
