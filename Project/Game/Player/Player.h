#pragma once

#include "3d/ViewProjection.h"

//object
#include"BaseObject/BaseObject.h"

/// Weapon
#include"Weapon/PlayerHandLeft.h"
#include"Weapon/PlayerHandRight.h"

/// utility
#include"utility/Editor/GlobalParameter.h"

/// behavior
#include"ComboAttackBehavior/ComboAttackFirst.h"
#include"PlayerBehavior/BasePlayerBehavior.h"

/// std
#include<array>
#include <memory>
#include<string>

/// <summary>
/// プレイヤークラス
/// </summary>
class LockOn;
class Player : public BaseObject{
private:

	struct ComboParm {
		float permissionTime;
	};
	
private:
	/// ===================================================
	///private variaus
	/// ===================================================
	
	/// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	const std::string groupName_ = "Player";      /// グループ名

	/// other class
	LockOn* pLockOn_;                            /// LockOnクラス 
	std::unique_ptr<PlayerHandLeft>leftHand_;    /// 左手
	std::unique_ptr<PlayerHandRight>rightHand_;  /// 右手
	
	/// move
	float objectiveAngle_;                       /// 目標角度
	Vector3 velocity_;                           /// 速度
	Vector3 prePos_;                             /// 移動前座標
										         
	/// jump							         
	float jumpSpeed_;	                         /// ジャンプ初速
	const float fallLimit_ = -1.2f;              /// ジャンプ
										         
										         
	bool isAttack_;                              /// 攻撃フラグ 
	float fallSpeed_;                  	         /// 落ちるスピード

	///* コンボパラメータ
	std::array<ComboParm, 2>normalComboParms_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;
	std::unique_ptr<BaseComboAattackBehavior>comboBehavior_ = nullptr;


public:
	static float InitY_;
	Player();

	/// ===================================================
	///  public method
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
	void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior);


	void AdjustParm();                      /// デバッグ
	void SetLockOn(LockOn* lockon);    /// ロックオンクラスをセット
	
	/// collision

	/// ダメージ
	void TakeDamage();                 /// ダメージ受ける     
	void DamageRendition();            /// ダメージリアクション

	
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
           
	/// ===================================================
	/// getter
	/// ===================================================
	PlayerHandLeft* GetLeftHand() const{ return leftHand_.get(); }
	PlayerHandRight* GetRightHand() const { return rightHand_.get(); }
	const bool& GetIsAttack()const { return isAttack_; }
	float GetMuzzulJumpSpeed()const { return jumpSpeed_; }
	BasePlayerBehavior* GetBehavior()const { return behavior_.get(); }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetIsAttack(bool is) { isAttack_ = is; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }


};
