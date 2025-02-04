#pragma once
#include"base/DirectXCommon.h"
#include"base/SrvManager.h"
#include"base/Material.h"

#include"3d/Object3d.h"
#include"3d/ViewProjection.h"
#include"3d/WorldTransform.h"

//
#include"ParticleEmitter.h"
#include"struct/ParticleForGPU.h"
//math
#include"MinMax.h"
#include"Box.h"
// std
#include<list>
#include<unordered_map>

enum class BlendMode;
class ParticleCommon;
struct ParticleEmitter::GroupParamaters;
struct ParticleEmitter::Parameters;
class ParticleManager {
private:
	
	///============================================================
	/// struct
	///============================================================

	struct Particle {
		float lifeTime_;
		float currentTime_;
		float gravity_;
		Vector3 direction_;
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
		ParticleEmitter::GroupParamaters parm;
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
		uint32_t instanceNum;
		ParticleFprGPU* instancingData;
		uint32_t textureHandle;
	};


private:

	///============================================================
	///private variant
	///============================================================

	//other class
	SrvManager* pSrvManager_;
	ParticleCommon* pParticleCommon_;
	AccelerationField accelerationField_;
	ViewProjection* viewProjection_;

public:
	std::unordered_map<std::string, ParticleGroup>particleGroups_;
public:

	static	ParticleManager* GetInstance();

	///============================================================
	///public method
	///============================================================

	//初期化、更新、描画
	void Init(SrvManager* srvManager);
	void Update(const ViewProjection& viewProjection);
	void Draw(const ViewProjection& viewProjection);
	float DirectionToEulerAngles(const Vector3& direction, const ViewProjection& view);

	 ViewProjection* SetViewProjection(const ViewProjection& view);

	/// グループ作成
	void CreateParticleGroup(const std::string name, const std::string modelFilePath,const std::string& extension, const uint32_t& maxnum);

	// モデル、リソース作成
	void SetModel(const std::string& name,const std::string& modelName, const std::string& extension);
	void CreateMaterialResource(const std::string& name);
	void CreateInstancingResource(const std::string& name, const uint32_t& instanceNum);

	// テクスチャセット
	void SetTextureHandle(const std::string name, const uint32_t& handle);

	/// リセット
	void ResetAllParticles();

	// 作成
	Particle  MakeParticle(const ParticleEmitter::Parameters&paramaters);
	
	//　エミット
	void Emit(
		std::string name, const ParticleEmitter::Parameters&
		paramaters,const ParticleEmitter::GroupParamaters&groupParamaters, const int32_t& count);

};