#pragma once

#include "3d/ViewProjection.h"

//class
#include"BaseObject/BaseObject.h"

/// utility
#include"utility/Editor/GlobalParameter.h"

/// behavior
#include"ComboAttackBehavior/ComboAttackFirst.h"
#include"PlayerBehavior/BasePlayerBehavior.h"

/// std
#include<array>
#include <memory>

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
	
	/// other class
	LockOn* pLockOn_;                   /// LockOnクラス 
	GlobalParameter* globalParameter_;  /// グローバルパラメータ
	const std::string groupName_="Player";

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

	///* コンボに関するパラメータ
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
	void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior);


	void ImguiParmUpdate();                      /// デバッグ
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
	const bool& GetIsAttack()const { return isAttack_; }
	float GetMuzzulJumpSpeed()const { return muzzelJumpSpeed_; }
	BasePlayerBehavior* GetBehavior()const { return behavior_.get(); }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetIsAttack(bool is) { isAttack_ = is; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }


};
