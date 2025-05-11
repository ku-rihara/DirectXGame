#pragma once

#include "3d/ViewProjection.h"

//object
#include"BaseObject/BaseObject.h"
#include"Effect/ImpactEffect.h"

/// Parts
#include"Parts/PlayerHandLeft.h"
#include"Parts/PlayerHandRight.h"

/// utility
#include"utility/ParameterEditor/GlobalParameter.h"
#include"utility/ParticleEditor/ParticleEmitter.h"

/// behavior
#include"ComboAttackBehavior/RightJobPunch.h"
#include"PlayerBehavior/BasePlayerBehavior.h"
#include"TitleBehavior/BaseTitleBehavior.h"

/// collider 
#include"Collider/AABBCollider.h"

/// std
#include<array>
#include <memory>
#include<string>

/// <summary>
/// プレイヤークラス
/// </summary>
class LockOn;
class GameCamera;
class Player : public BaseObject, public AABBCollider {


public:
	enum class ComboNum {
		FIRST,
		SECOND,
		THIRD,
		FORTH,
	};

private:

	struct UpperParm {
		float BackLashValue;
		float BackLashEaseTime;
	};
	struct JumpParm {
		float jumpSpeed;
		float gravity;
		float fallSpeedLimit;
	};

	struct ComboParm {
        float waitTime;
        float attackEaseMax;
        float attackReach;
    };

	///Paramater構造体
	struct PlayerParams {
		Vector3 startPos_;
		float rushDistance;
		float rushEaseMax;
		float upperPosY;
		UpperParm upperParm;
		JumpParm normalJump;
		JumpParm bountJump;
		JumpParm upperJump;
		float moveSpeed;
		float fallSpeed;
		float attackRotate;
		float attackRotateAnit;
		float attackRotateEaseT;
		float attackFloatValue;
		float attackFloatEaseT;
	};

	struct ParticleEffect {
		std::string name;
		std::unique_ptr<ParticleEmitter>emitter;
	};

private: 

	/// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	const std::string groupName_ = "Player";      /// グループ名

	/// behavior
	std::unique_ptr<BasePlayerBehavior>behavior_ = nullptr;
	std::unique_ptr<BaseComboAattackBehavior>comboBehavior_ = nullptr;
	std::unique_ptr<BaseTitleBehavior>titleBehavior_ = nullptr;

	/// Particle
	uint32_t cirlceTexture_;
	std::unique_ptr<ParticleEmitter>fallCrack_;
	std::array<ParticleEffect,1> debriParticle_;
    std::array<ParticleEffect, 1> rushParticle_;
	std::array<ParticleEffect, 3> starEffect_;
   

	/// other class
	LockOn* pLockOn_;           /// LockOnクラス
	GameCamera* pGameCamera_;   /// ゲームカメラポインタ
	std::unique_ptr<PlayerHandLeft>    leftHand_;      /// 左手
	std::unique_ptr<PlayerHandRight>   rightHand_;     /// 右手
	std::unique_ptr<Object3d>          headObj_;       /// 頭
	WorldTransform                     headTransform_; /// 頭トランスフォーム
    std::list<std::unique_ptr<ImpactEffect>> effects_; /// エフェクト


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


	///sound 
	int punchSoundID_;
	int strongPunch_;
	int fallSound_;
	int starSound_;

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
	void    Move(const float& speed);  /// 移動
	bool    GetIsMoving();             /// 動かしてるかフラグ
	void    MoveToLimit();             /// 移動制限
	Vector3 GetInputDirecton();        /// 入力による速度
	void    UpdateMatrix();            ///　行列更新

	//* ジャンプ                             
	void Jump(float& speed, const float& fallSpeedLimit, const float& gravity);                              /// ジャンプ
	void Fall(float& speed, const float& fallSpeedLimit, const float& gravity, const bool& isJump = false);  /// 落ちる

	///* 振る舞い切り替え
	void ChangeBehavior(std::unique_ptr<BasePlayerBehavior>behavior);
	void ChangeComboBehavior(std::unique_ptr<BaseComboAattackBehavior>behavior);
	void ChangeTitleBehavior(std::unique_ptr<BaseTitleBehavior>behavior);
	void ChangeCombBoRoot();

	//* エフェクト
    void ParticleInit();
    void FallEffectInit(const Vector3& pos);

	void DebriParticleEmit();
	void FallEffectUpdate();
	void StartEffectEmit();
	void ParticleUpdate();
    void RushParticleUdate();

	///* 向き
	void FaceToTarget();
	void AdaptRotate();

	/// ====================================================================
	/// Editor
	/// ====================================================================
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AdjustParm();

	/// ====================================================================
	/// Collision
	/// ====================================================================
	void OnCollisionStay([[maybe_unused]] BaseCollider* other) override;
	Vector3 GetCollisionPos() const override;

	/// <summary>
	/// sound
	/// </summary>
	void SoundPunch();
	void SoundStrongPunch();
	void FallSound();

	/// =========================================================================================
	/// getter
	/// =========================================================================================
	PlayerHandLeft*     GetLeftHand()        const { return leftHand_.get(); }
	PlayerHandRight*    GetRightHand()       const { return rightHand_.get(); }
	LockOn*             GetLockOn()          const { return pLockOn_; }
	GameCamera*         GetGameCamera()      const { return pGameCamera_; }
	PlayerParams           GetParamater()    const { return playerParams_; }
	BasePlayerBehavior* GetBehavior()        const { return behavior_.get(); }
	BaseTitleBehavior*  GetTitleBehavior()   const { return titleBehavior_.get(); }
	uint32_t            GetCircleTexture()   const { return cirlceTexture_; }
	/// =========================================================================================
	/// setter
	/// =========================================================================================
	void SetViewProjection(const ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetLockOn(LockOn* lockon);
	void SetGameCamera(GameCamera* gamecamera);
	void SetTitleBehavior();
	void SetRotateInit();
	void SetLightPos();
	void HeadLightSetting();

	///* 
	ComboParm GetNormalComboParm(const ComboNum& index)const { return normalComboParms_[static_cast<int>(index)]; }
	ComboParm GetJumpComboParm(const ComboNum& index)const { return jumpComboParms_[static_cast<int>(index)]; }

	void SetHeadPosY(const float& posy) { headTransform_.translation_.y = posy; }
	void SetHeadRotateX(const float& zrotate) { headTransform_.rotation_.x = zrotate; }
	void SetHeadRotateY(const float& zrotate) { headTransform_.rotation_.y = zrotate; }
	void SetHeadScale(const Vector3& scale) { headTransform_.scale_ = scale; }
};