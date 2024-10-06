#pragma once
//#include"Model.h"
#include"BaseObject3d.h"
#include <memory>
#include<random>
#include<vector>

class Object3dParticle :public BaseObject3d {
public:
	uint32_t instanceMax_;
	uint32_t instanceNum_;//インスタンス数
	D3D12_CPU_DESCRIPTOR_HANDLE  instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE  instancingSrvHandleGPU_;

	//リソースとデータ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> instancingResources_;
	ParticleFprGPU* instancingData_;
	//パーティクル変数
	const float kDeltaTime_ = 1.0f / 60.0f;
	std::vector<float>lifeTimes_;
	std::vector<float>currentTimes_;
	std::vector<Vector3>velocities_;
	//ワールドトランスフォーム
	std::vector<std::unique_ptr<WorldTransform>> worldTransforms_;
	std::vector<Vector4>colors_;
private:
	/// <summary>
	/// リソース作成
	/// </summary>
	/// <param name="instanceNum"></param>
	/// <param name="randomEngine"></param>
	/// <param name="dist"></param>
	void CreateInstancingResource(const uint32_t& instanceNum, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist);
	/// <summary>
	/// サイズ確保
	/// </summary>
	void SizeSecure(const uint32_t& instanceNumMax);

public:
	/// <summary>
	/// モデル生成
	/// </summary>
	/// <param name="instanceName"></param>
	/// <param name="instanceNum"></param>
	/// <param name="randomEngine"></param>
	/// <param name="dist"></param>
	/// <returns></returns>
	static Object3dParticle* CreateModel(const std::string& instanceName, const std::string& extension, const uint32_t& instanceNum, std::mt19937& randomEngine, std::uniform_real_distribution<float> dist);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(std::optional<const ViewProjection*>viewProjection=std::nullopt);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	/// <param name="textureHandle"></param>
	void Draw(const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
	/// <summary>
	/// Debug表示
	/// </summary>
	void DebugImgui()override;
	
	/// <summary>
	/// ランダム生成
	/// </summary>
	/// <param name="random"></param>
	void MakeParticle(std::mt19937& random);

	//getter
	uint32_t GetKnumInstance()const { return instanceNum_; }

};