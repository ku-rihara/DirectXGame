#pragma once

#include"utility/Editor/GlobalParameter.h"
#include"BaseObject/BaseObject.h"
#include"utility/Particle/ParticleEmitter.h"

class BasePlayerHand :public BaseObject {
protected:

	///=============================================
	/// private variant
	///=============================================

    /// グローバルなパラメータ
	GlobalParameter* globalParameter_;            /// グローバルパラメータ
	std::string groupName_;                      /// グループ名

	// エミッター
	

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

	virtual void SaveAndLoad();
};