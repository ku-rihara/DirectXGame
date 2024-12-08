#pragma once
#include"base/DirectXCommon.h"
#include"base/SrvManager.h"
#include"base/Material.h"

#include"3d/Object3d.h"
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"

//
#include"struct/ParticleForGPU.h"
//math
#include"MinMax.h"
#include"Box.h"
// std
#include<list>
#include<unordered_map>

class ParticleCommon;
class ParticleManager {
private:

	///=========================================
	/// struct
	///=========================================

	struct Particle {
		float lifeTime_;
		float currentTime_;
		float gravity_;
		Vector3 velocity_;
		Vector3 rotateSpeed_;
		Vector4 color_;
		WorldTransform worldTransform_;

	};

	struct AccelerationField {///　加速フィールド
		Vector3 acceleration;
		AABB area;
		bool isAdaption;
	};

	struct ParticleGroup {/// パーティクルグループ
		Model* model;
		Material material;
		std::list<Particle>particles;
		uint32_t srvIndex;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
		uint32_t instanceNum;
		ParticleFprGPU* instancingData;
		uint32_t textureHandle;
	};


private:

	///=========================================
	///private variant
	///=========================================

	//other class
	SrvManager* pSrvManager_;
	ParticleCommon* pParticleCommon_;

	AccelerationField accelerationField_;
	

public:
	std::unordered_map<std::string, ParticleGroup>particleGroups_;


public:

	static	ParticleManager* GetInstance();

	///==============================================
	///public method
	///==============================================

		//初期化
	void Init(SrvManager* srvManager);
	void Update(std::optional<const ViewProjection*> viewProjection);
	void Draw(const ViewProjection& viewProjection);

	void SetTextureHandle(const std::string name,const uint32_t& handle);

	void CreateParticleGroup(const std::string name, const std::string modelFilePath,const std::string& extension, const uint32_t& maxnum);

	// モデル、マテリアル作成
	void SetModel(const std::string& name,const std::string& modelName, const std::string& extension);
	void CreateMaterialResource(const std::string& name);

	void CreateInstancingResource(const std::string& name, const uint32_t& instanceNum);

	Particle  MakeParticle(
		const Vector3& basePosition, const V3MinMax& positionDist,
		const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
		const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
		const Vector3& baseRotate, const Vector3& baseRotateSpeed, const V3MinMax& RotateDist,
		const V3MinMax& rotateSpeedDist);

	void Emit(
		std::string name, const Vector3& basePosition, const V3MinMax& positionDist,
		const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
		const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
		const Vector3&baseRotate,const Vector3&baseRotateSpeed,const V3MinMax& RotateDist,
		const V3MinMax& rotateSpeedDist, uint32_t count);

};