#pragma once

#include "3d/ViewProjection.h"

//object
#include"BaseObject/BaseObject.h"
#include"Effect/Effect.h"

/// Weapon
#include"Weapon/PlayerHandLeft.h"
#include"Weapon/PlayerHandRight.h"

/// utility
#include"utility/Editor/GlobalParameter.h"
#include"utility/Particle/ParticleEmitter.h"

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
public:
	enum NormalComboNum {
		FIRST,
		SECOND,
		THIRD,
		FORTH,
	};

	enum JumpComboNum {
		JFIRST,
		JSECOND,
	};

private:

	struct ComboParm {
		float permissionTime;
		float punchEaseMax;
		float punchReach;
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
	std::list<std::unique_ptr<Effect>> effects_;

	/// move
	float objectiveAngle_;                       /// 目標角度
	Vector3 direction_;                          /// 速度
	Vector3 prePos_;                             /// 移動前座標
	float moveSpeed_;                            ///移動速度

										         
	/// jump							         
	const float fallSpeedLimit_ = -1.2f;              /// ジャンプ
	float gravity_;
										         										         
	bool isAttack_;                              /// 攻撃フラグ 
	float fallSpeed_;                  	         /// 落ちるスピード

	/// paramater
	float rushDistance_;                           /// 突進距離
	float rushEaseMax_;                            /// 突進スピード
	float jumpSpeed_;	                           /// ジャンプ初速
	float upperPosY_;                              /// アッパー時の高さ

	///* コンボパラメータ
	std::array<ComboParm, 4>normalComboParms_;
	std::array<ComboParm, 2>jumpComboParms_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;
	std::unique_ptr<BaseComboAattackBehavior>comboBehavior_ = nullptr;

	std::string fallParticleName_;
	std::unique_ptr<ParticleEmitter>fallEmitter_;


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
	Vector3 GetInputDirecton();         /// 入力による速度
	void MoveToLimit();                 /// 移動制限

	void FallParticleEmit();
	void Jump(float& speed);           /// ジャンプ
	void Fall();                       /// 落ちる

	/// 振る舞い切り替え
	void ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior);
	void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior);

	void FallEffectUpdate();
	void FallEffectInit(const Vector3& pos);
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
	LockOn* GetLockOn() const { return pLockOn_; }
	const bool& GetIsAttack()const { return isAttack_; }
	float GetMuzzulJumpSpeed()const { return jumpSpeed_; }
	float GetMoveSpeed()const { return moveSpeed_; }
	float GetUpperPosY()const { return upperPosY_; }
	BasePlayerBehavior* GetBehavior()const { return behavior_.get(); }
	/// ===================================================
	/// setter
	/// ===================================================
	void SetIsAttack(bool is) { isAttack_ = is; }
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	/*void SetUpperPosY(const float& posy) { upperPosY_ = posy; }*/

	/// comboParamater
	float GetRushDistance()const { return rushDistance_; }
	float GetRushEaseMax()const { return rushEaseMax_; }

	float GetWaitTime(NormalComboNum index)const;
	float GetPunchEaseMax(NormalComboNum index)const;
	float GetPunchReach(NormalComboNum index)const;

	float GetJWaitTime(JumpComboNum index)const;
	float GetJPunchEaseMax(JumpComboNum index)const;
	float GetJPunchReach(JumpComboNum index)const;
};
