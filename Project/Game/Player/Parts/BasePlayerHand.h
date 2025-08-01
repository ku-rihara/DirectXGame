#pragma once

#include"utility/ParameterEditor/GlobalParameter.h"
#include"utility/RailEditor/ControlPosManager.h"
#include"utility/RailEditor/RailManager.h"

#include"BaseObject/BaseObject.h"
#include"utility/ParticleEditor/ParticleEmitter.h"

class BasePlayerHand :public BaseObject {
protected:

	///=============================================
	/// private variant
	///=============================================

    /// グローバルなパラメータ
	GlobalParameter* globalParameter_;           
	std::string groupName_;                      

	// エミッター
	std::unique_ptr<ParticleEmitter>emitter_;
	Vector3 direction_;

	
	/// paramater
	float railRunSpeedThree_;
	float railRunSpeedForth_;

public:
    virtual ~BasePlayerHand() = default;
	///============================================
	/// public method
	///=============================================

	virtual void Init();
	virtual void Update();

	virtual void AdjustParm() = 0;

	virtual void DissolveAdapt(const float& dissolve);

	///-------------------------------------------------------------------------------------
	///Editor
	///-------------------------------------------------------------------------------------
	void ParamLoadForImGui();
	void AddParmGroup();
	void SetValues();
	void ApplyGlobalParameter();
	void AjustParmBase();

	virtual void SaveAndLoad();

	///========================================================
	/// getter method
    ///========================================================
	Vector3 GetDirection()const { return direction_; }
    const WorldTransform& GetObjTransform() const { return obj3d_->transform_; }
	float GetRailRunSpeedThree()const { return railRunSpeedThree_; }
	float GetRailRunSpeedForth()const { return railRunSpeedForth_; }

	///========================================================
	/// setter method
    ///========================================================
	virtual void SetRailParent(WorldTransform* parent)=0;
	virtual void SetParent(WorldTransform* parent);
	void SetBlendModeSub();
	void SetBlendModeAdd();
    void SetObjTranslate(const Vector3& pos) { obj3d_->transform_.translation_ = pos; }
};