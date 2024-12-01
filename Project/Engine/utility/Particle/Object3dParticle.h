//#pragma once
////#include"Model.h"
////#include"BaseObject3d.h"
//#include <memory>
//#include"random.h"
//#include<list>
//#include"struct/Transform.h"
////math
//#include"MinMax.h"
//#include"Box.h"
//
//class Object3dParticle :public BaseObject3d {
//public:
//	struct AccelerationField  {
//		Vector3 acceleration;
//		AABB area;
//		bool isAdaption;
//	};
//		
//	struct Emitter {
//		Transform transform;
//		uint32_t count;
//		float frequency;
//		float frequencyTime;
//	};
//	//struct Particle {
//	//	float lifeTime_;
//	//	float currentTime_;
//	//	Vector3 velocity_;
//	//	Vector4 color_;
//	//	WorldTransform worldTransform_;
//	//
//	//};
//	 std::list<Particle>particles_;
//	 Emitter emitter_;
//	 AccelerationField accelerationField_;
//private:
//	uint32_t instanceMax_;
//	uint32_t instanceNum_;//インスタンス数
//	D3D12_CPU_DESCRIPTOR_HANDLE  instancingSrvHandleCPU_;
//	D3D12_GPU_DESCRIPTOR_HANDLE  instancingSrvHandleGPU_;
//
//		ParticleFprGPU* instancingData_;
//	//パーティクル変数
//	const float kDeltaTime_ = 1.0f / 60.0f;
//	
//private:
//	/// <summary>
//	/// リソース作成
//	/// </summary>
//	/// <param name="instanceNum"></param>
//	/// <param name="randomEngine"></param>
//	/// <param name="dist"></param>
//	void CreateInstancingResource(const uint32_t& instanceNum);
//	/// <summary>
//	/// サイズ確保
//	/// </summary>
//	void Clear();
//	/// <summary>
//	/// ランダム生成
//	/// </summary>
//	/// <param name="random"></param>
//	Particle  MakeParticle(MinMax dist, MinMax velocityDist, const Transform& transform, float lifeTime);
//
//public:
//	/// <summary>
//	/// モデル生成
//	/// </summary>
//	/// <param name="instanceName"></param>
//	/// <param name="instanceNum"></param>
//	/// <param name="randomEngine"></param>
//	/// <param name="dist"></param>
//	/// <returns></returns>
//	static Object3dParticle* CreateModel(const std::string& instanceName, const std::string& extension, const uint32_t& instanceNum);
//	/// <summary>
//	/// 更新
//	/// </summary>
//	void Update(std::optional<const ViewProjection*>viewProjection=std::nullopt);
//	/// <summary>
//	/// 描画
//	/// </summary>
//	/// <param name="viewProjection"></param>
//	/// <param name="textureHandle"></param>
//	void Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
//	/// <summary>
//	/// Debug表示
//	/// </summary>
//	void DebugImgui()override;
//	// マテリアルリソース作成
//	void CreateMaterialResource()override;
//	
//	/// <summary>
//	/// エミッター
//	/// </summary>
//	/// <param name="emitter"></param>
//	/// <param name="dist"></param>
//	/// <param name="velocityDist"></param>
//	/// <param name="lifeTime"></param>
//	void Emit(const Emitter& emitter, MinMax dist, MinMax velocityDist, float lifeTime);
//
//	//getter
//	uint32_t GetKnumInstance()const { return instanceNum_; }
//public:
//	
//};