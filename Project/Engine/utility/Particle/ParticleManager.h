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
		BlendMode blendMode_;
		uint32_t srvIndex;
		bool isBillBord;
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
	

public:
	std::unordered_map<std::string, ParticleGroup>particleGroups_;
public:

	static	ParticleManager* GetInstance();

	///============================================================
	///public method
	///============================================================

	//初期化、更新、描画
	void Init(SrvManager* srvManager);
	void Update(std::optional<const ViewProjection*> viewProjection);
	void Draw(const ViewProjection& viewProjection);

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
	Particle  MakeParticle(
		const Vector3& basePosition, const V3MinMax& positionDist,
		const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
		const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
		const Vector3& baseRotate, const V3MinMax& RotateDist,
		const V3MinMax& rotateSpeedDist, const bool& isRotateforDirection);
	
	//　エミット
	void Emit(
		std::string name, const Vector3& basePosition, const V3MinMax& positionDist,
		const FMinMax& scaledist, const V3MinMax& velocityDist, const Vector4& baseColor,
		const V4MinMax& colorDist, const float& lifeTime, const float& gravity,
		const Vector3&baseRotate,const V3MinMax& RotateDist,
		const V3MinMax& rotateSpeedDist, uint32_t count,const bool&isbillbord, const bool& isRotateforDirection, const BlendMode& blendmode);

};