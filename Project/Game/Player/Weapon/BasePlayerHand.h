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

	//　移動用制御点
	std::unique_ptr<RailManager> railManager_; /// レールマネージャ

public:
	/*virtual ~BasePlayerHand() = default;*/
	///============================================
	/// public method
	///=============================================

	virtual void Init();
	virtual void Update();
	
	virtual void Draw(const ViewProjection& viewprojection);

	virtual void RailUpdate() = 0;
	virtual void AdjustParm() = 0;

	///-------------------------------------------------------------------------------------
	///Editor
	///-------------------------------------------------------------------------------------
	void ParmLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();

	virtual void SaveAndLoad();

	///============================================
	/// getter method
	///=============================================
	Vector3 GetDirection()const { return direction_; }
	RailManager* GetRailManager() { return railManager_.get(); }
	///============================================
	/// setter method
	///=============================================
	
	virtual void SetParent(WorldTransform* parent);
};