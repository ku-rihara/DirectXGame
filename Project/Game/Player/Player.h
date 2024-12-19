#pragma once

#include "3d/ViewProjection.h"

//class
#include"BaseObject/BaseObject.h"
#include"Collider/AABBCollider.h"
/// behavior
#include"PlayerBehavior/BasePlayerBehavior.h"

#include <memory>

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
	
	/// move
	float objectiveAngle_;              /// 目標角度
	Vector3 velocity_;                  /// 速度
	Vector3 prePos_;                    /// 移動前座標
	
	/// jump
	float jumpSpeed_;	                /// ジャンプスピード
	float muzzelJumpSpeed_;	            /// ジャンプ初速
	const float fallLimit_ = -1.2f;     /// ジャンプ


	bool isAttack_;                     /// 攻撃フラグ 
	float fallSpeed_;                  	/// 落ちるスピード

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;


public:
	static float InitY_;
	Player();

	/// ===================================================
	///public method
	/// ===================================================

	//初期化、更新、描画
	void Init() override;
	void Update() override;
	void Draw(const ViewProjection& viewProjection) override;

	/// 移動
	void Move(const float& speed);      /// 移動
	bool GetIsMoving();                 /// 動かしてるかフラグ
	Vector3 GetInputVelocity();         /// 入力による速度
	void MoveToLimit();                 /// 移動制限

	
	void Jump(float& speed);           /// ジャンプ
	void Fall();                       /// 落ちる

	/// 振る舞い切り替え
	void ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior);
	 
	void Debug();                      /// デバッグ
	void SetLockOn(LockOn* lockon);    /// ロックオンクラスをセット
	
	/// collision

	/// ダメージ
	void TakeDamage();                 
	void DamageRendition();
           
	/// ===================================================
	/// getter
	/// ===================================================
	const bool& GetIsAttack()const { return isAttack_; }
	float GetMuzzulJumpSpeed()const { return muzzelJumpSpeed_; }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetIsAttack(bool is) { isAttack_ = is; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }


};
