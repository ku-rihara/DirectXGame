#pragma once

#include "3d/ViewProjection.h"
#include "utility/RailEditor/RailManager.h"
#include"Pipeline/ParticlePipeline.h"

/// primitive
#include "Primitive/IPrimitive.h"
#include"3d/Line3D.h"

#include "ParticleParameter.h"
#include <string>
#include <vector>


class ParticleEmitter : public ParticleParameter { 
private:

	float currentTime_;


	/*std::unique_ptr<Object3d> obj3d_;*/
    Line3D debugLine_;
	WorldTransform emitBoxTransform_;

	bool isStartRailMove_;
    
	std::string editorMessage_;

public:
	// コンストラクタ
	ParticleEmitter()=default;
	~ParticleEmitter() override = default;

	//Create
	static ParticleEmitter* CreateParticle(
		const std::string& name, const std::string& modelFilePath,
		 const int32_t& maxnum);

	static ParticleEmitter* CreateParticlePrimitive(
        const std::string& name, const PrimitiveType&primitiveType, const int32_t& maxnum);

	// 初期化
	void Init();

	void Emit();
    void StartRailEmit();

	void UpdateEmitTransform();
	void Update();
	void RailDraw(const ViewProjection& viewProjection);
	void DebugDraw(const ViewProjection& viewProjection);

	/// editor
	void EditorUpdate()override;
	
	private:
    void RailMoveUpdate();

    public:
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
	void SetBlendMode(const BlendMode& blendmode);
	void SetBillBordType(const BillboardType& billboardType);
	void SetParentBasePos(WorldTransform* parent);
	void SetIsMoveForRail(const bool& isMoveForRail) { isMoveForRail_ = isMoveForRail; }
	void SetMoveSpeed(const float& moveSpeed) { moveSpeed_ = moveSpeed; }
	void SetTargetPosition(const Vector3& targetPos) { parameters_.targetPos = targetPos; }


};