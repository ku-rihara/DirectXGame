#pragma once

#include "3d/ViewProjection.h"
#include "utility/RailEditor/RailManager.h"
#include "utility/ParticleEditor/ParticleCommon.h"

/// primitive
#include "Primitive/IPrimitive.h"
#include"3d/Mesh.h"

#include "ParticleParameter.h" // ParticleParameterのヘッダーをインクルード
#include <string>
#include <vector>

class ParticleEmitter : public ParticleParameter { // ParticleParameterを継承
private:

	float currentTime_;


	std::unique_ptr<Object3d> obj3d_;
	WorldTransform emitBoxTransform_;

	bool isMoveForRail_;
	float moveSpeed_;
	std::unique_ptr<RailManager> railManager_;

	std::string editorMessage_;

public:
	// コンストラクタ
	ParticleEmitter();
	~ParticleEmitter() override = default;

	//Create
	static ParticleEmitter* CreateParticle(
		const std::string& name, const std::string& modelFilePath,
		const std::string& extension, const int32_t& maxnum);

	static ParticleEmitter* CreateParticlePrimitive(
        const std::string& name, const PrimitiveType&primitiveType, const int32_t& maxnum);

	// 初期化
	void Init();

	void Emit();
	void UpdateEmitTransform();
	void Update();
	void RailDraw(const ViewProjection& viewProjection);
	void DebugDraw(const ViewProjection& viewProjection);

	/// editor
	void EditorUpdate();
	void ParticleChange();
	void ScaleParmEditor();
	void DisplayFileSelection(const std::string& header, const std::vector<std::string>& filenames, int& selectedIndex,
		const std::function<void(const std::string&)>& onApply);

	// load save
	void  ParmSaveForImGui()override;
	void  ParmLoadForImGui()override;
	void ApplyGlobalParameter(const std::string& particleName)override;
	void ApplyTexture(const std::string& texturename)override;
	void SetValues()override;
	void ImGuiTextureSelection();


	///=================================================================================
	/// getter method
	///=================================================================================
	bool GetIsMoveForRail()const { return isMoveForRail_; }
	float GetMoveSpeed()const { return moveSpeed_; }

	///=================================================================================
	/// setter method
	///=================================================================================
	void SetTextureHandle(const uint32_t& hanle);
	void SetParentTransform(const WorldTransform* transform);
	void SetFollowingPos(const Vector3* pos);
	void SetBlendMode(const ParticleCommon::BlendMode& blendmode);
	void SetBillBordType(const WorldTransform::BillboardType& billboardType);
	void SetParentBasePos(WorldTransform* parent);
	void SetIsMoveForRail(const bool& isMoveForRail) { isMoveForRail_ = isMoveForRail; }
	void SetMoveSpeed(const float& moveSpeed) { moveSpeed_ = moveSpeed; }
	void SetTargetPosition(const Vector3& targetPos) { parameters_.targetPos = targetPos; }


};