#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>

#include <optional>
#include<string>
#include<map>
#include <memory>
#include<vector>
#include"Vector4.h"
#include"Vector3.h"
#include"Vector2.h"
//struct
#include"TransformationMatrix.h"
#include"ModelData.h"
#include"Material.h"
#include "MaterialData.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class TextureManager;
class Model{
private:
	//static std::map<std::string, std::unique_ptr<Model>> modelInstances;
	const uint32_t kNumInstance_ = 10;//インスタンス数
	D3D12_CPU_DESCRIPTOR_HANDLE  instancingSrvHandleCPU_;
	D3D12_GPU_DESCRIPTOR_HANDLE  instancingSrvHandleGPU_;
	//テクスチャ
	TextureManager* textureManager_=nullptr;
	uint32_t textureHandle_;
	ModelData modelData_;
	D3D12_GPU_DESCRIPTOR_HANDLE handle_;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//リソース******************************************************************
	//Material
	Microsoft::WRL::ComPtr<ID3D12Resource >materialResource_;
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_;
	//wvpリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>wvpResource_;
	//indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResource_;

	//データ****************************************************************************
	TransformationMatrix* wvpDate_;
	TransformationMatrix* instancingData_;

	Material* materialDate_;
	
	//後に消すかも
	bool useMonsterBall = true;
public:
	static Model* Create(const std::string& instanceName);

	static Model* CreateParticle(const std::string& instanceName);
	
static	void PreDraw(ID3D12GraphicsCommandList* commandList);
	
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	/// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel(const std::string& ModelName);
	/// <summary>
	/// モデルパーティクル作成
	/// </summary>
	/// <param name="ModelName"></param>
	void CreateModelParticle(const std::string& ModelName);
	/// <summary>
	/// モデル作成共通
	/// </summary>
	/// <param name="ModelName"></param>
	void CreateCommon(const std::string& ModelName);
	/// <summary>
	/// 球描画
	/// </summary>
	void CreateSphere();
	/// <summary>
	/// モデル描画
	/// </summary>
	void Draw(const WorldTransform&worldTransform,const ViewProjection&viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);
	
	/// <summary>
	/// モデルバーティクル
	/// </summary>
	void DrawParticle(const std::vector<std::unique_ptr<WorldTransform>>& worldTransforms, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle = std::nullopt);

#ifdef _DEBUG
	void DebugImGui();
#endif

	//getter
	ModelData GetModelData()const { return modelData_; }
	uint32_t GetKnumInstance()const { return kNumInstance_; }

	//setter
	void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
	void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }
};
