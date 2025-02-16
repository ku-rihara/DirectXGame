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
#include"TitleBehavior/BaseTitleBehavior.h"

/// std
#include<array>
#include <memory>
#include<string>
#include <variant>

/// <summary>
/// プレイヤークラス
/// </summary>
class LockOn;
class GameCamera;
class Player : public BaseObject {


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
	///Paramater構造体
	struct PlayerParams {
		Vector3 startPos_;
		float jumpSpeed;
		float rushDistance ;
		float rushEaseMax  ;
		float upperPosY ;
		float moveSpeed ;
		float gravity ;
		float fallSpeed;
		float fallSpeedLimit;
		float attackRotate;
		float attackRotateEaseT;
	};

	struct ComboParm {
		float permissionTime;
		float punchEaseMax;
		float punchReach;
	};

private: ///*other class
	
	/// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	const std::string groupName_ = "Player";      /// グループ名

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;
	std::unique_ptr<BaseComboAattackBehavior>comboBehavior_ = nullptr;
	std::unique_ptr<BaseTitleBehavior>titleBehavior_ = nullptr;

	std::string fallParticleName_;
	std::unique_ptr<ParticleEmitter>fallEmitter_;

	/// other class
	LockOn*                            pLockOn_;     /// LockOnクラス
	GameCamera*                        pGameCamera_;
	std::unique_ptr<PlayerHandLeft>    leftHand_;     /// 左手
	std::unique_ptr<PlayerHandRight>   rightHand_;    /// 右手
	std::unique_ptr<Object3d>          headObj_;
	WorldTransform                     headTransform_;
	std::list<std::unique_ptr<Effect>> effects_;

private:
	/// ===================================================
	///private variaus
	/// ===================================================

	/// move
	float   objectiveAngle_;                     /// 目標角度
	Vector3 direction_;                          /// 速度
	Vector3 prePos_;                             /// 移動前座標

	///* コンボパラメータ
	std::array<ComboParm, 4>normalComboParms_;
	std::array<ComboParm, 2>jumpComboParms_;

	// カメラのビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	uint32_t cirlceTexture_;

	///sound 
	int punchSoundID_;
	int strongPunch_;
	int fallSound_;

	///PlayerParameter
	PlayerParams playerParams_;

public:
	static float InitY_;
	Player();

	/// ===================================================
	///  public method
	/// ===================================================

	///* 初期化、更新、描画
	void Init() override;
	void Update() override;
	void TitleUpdate();
	void Draw(const ViewProjection& viewProjection) override;
	void EffectDraw(const ViewProjection& viewProjection);

	///* 移動
	void    Move(const float& speed);         /// 移動
	bool    GetIsMoving();                    /// 動かしてるかフラグ
	void    MoveToLimit();                    /// 移動制限
	Vector3 GetInputDirecton();               /// 入力による速度
	void    UpdateMatrix();                   ///　行列更新

	//* ジャンプ                             
	void Jump(float& speed);                              /// ジャンプ
	void Fall(float& speed, const bool& isJump = false);  /// 落ちる

	///* 振る舞い切り替え
	void ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior);
	void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior);
	void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior>behavior);

	//* エフェクト
	void FallParticleEmit();
	void FallEffectUpdate();
	void FallEffectInit(const Vector3& pos);
             
	
	///* ダメージ
	void TakeDamage();          /// ダメージ受ける     
	void DamageRendition();     /// ダメージリアクション

	///* 向き
	void FaceToTarget();

	/// ===================================================
	/// Editor
	/// ===================================================
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AdjustParm();

	/// <summary>
	/// sound
	/// </summary>
	void SoundPunch();
	void SoundStrongPunch();
	void FallSound();

	/// =========================================================================================
	/// getter
	/// =========================================================================================
	PlayerHandLeft*     GetLeftHand()        const        { return leftHand_.get();  }
	PlayerHandRight*    GetRightHand()       const        { return rightHand_.get(); }
	LockOn*             GetLockOn()          const        { return pLockOn_;   }
	GameCamera*         GetGameCamera()      const        {return pGameCamera_;}
	PlayerParams        GetPlayerParams()    const        { return playerParams_; }
	BasePlayerBehavior* GetBehavior()        const        { return behavior_.get();}
	BaseTitleBehavior*  GetTitleBehavior()   const        { return titleBehavior_.get(); }
	uint32_t            GetCircleTexture()   const        { return cirlceTexture_; }
	/// =========================================================================================
	/// setter
	/// =========================================================================================
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetLockOn        (LockOn* lockon); 
	void SetGameCamera    (GameCamera*gamecamera);
	void SetTitleBehavior ();

	float GetWaitTime      (NormalComboNum index)const;
	float GetPunchEaseMax  (NormalComboNum index)const;
	float GetPunchReach    (NormalComboNum index)const;

	float GetJWaitTime     (JumpComboNum index)const;
	float GetJPunchEaseMax (JumpComboNum index)const;
	float GetJPunchReach   (JumpComboNum index)const;

	void SetHeadRotateX(const float& zrotate) { headTransform_.rotation_.x = zrotate; }
	void SetHeadRotateY(const float& zrotate) { headTransform_.rotation_.y = zrotate; }
};