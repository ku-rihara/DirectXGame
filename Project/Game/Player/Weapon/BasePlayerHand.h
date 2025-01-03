#pragma once

#include"utility/Editor/GlobalParameter.h"
#include"utility/Editor/ControlPosManager.h"
#include"utility/Editor/RailManager.h"

#include"BaseObject/BaseObject.h"
#include"utility/Particle/ParticleEmitter.h"

class BasePlayerHand :public BaseObject {
protected:

	///=============================================
	/// private variant
	///=============================================

    /// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	std::string groupName_;                       /// グループ名

	// エミッター
	std::unique_ptr<ParticleEmitter>emitter_;
	Vector3 direction_;

	
	/// paramater
	float railRunSpeed_;

public:
	/*virtual ~BasePlayerHand() = default;*/
	///============================================
	/// public method
	///=============================================

	virtual void Init();
	virtual void Update();
	
	virtual void Draw(const ViewProjection& viewprojection);

	virtual void AdjustParm() = 0;

	///-------------------------------------------------------------------------------------
	///Editor
	///-------------------------------------------------------------------------------------
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AjustParmBase();

	virtual void SaveAndLoad();

	///============================================
	/// getter method
	///=============================================
	Vector3 GetDirection()const { return direction_; }
	float GetRailRunSpeed()const { return railRunSpeed_; }

	///============================================
	/// setter method
	///=============================================
	virtual void SetRailParent(WorldTransform* parent)=0;
	virtual void SetParent(WorldTransform* parent);
};