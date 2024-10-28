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
#include<numbers>
//assimp
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
//struct
#include"TransformationMatrix.h"
#include"ModelData.h"
#include"Material.h"
#include "MaterialData.h"
#include "ParticleForGPU.h"

//class
#include "WorldTransform.h"
#include "ViewProjection.h"
#include"ObjectColor.h"
#include"DirectXCommon.h"

//3Dモデル共通部
class ModelCommon {
private:
	DirectXCommon* dxCommon_;
public:
	void Init(DirectXCommon* dxCommon);

	//getter
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
};

class TextureManager;
class Model {
private:
	////ModelCommonのポインタ
	//ModelCommon* modelCommon_;
	//テクスチャ
	TextureManager* textureManager_ = nullptr;
	uint32_t textureHandle_;
	ModelData modelData_;
	
	D3D12_GPU_DESCRIPTOR_HANDLE handle_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	//リソース******************************************************************
	//Material
	/*Microsoft::WRL::ComPtr<ID3D12Resource >materialResource_;*/
	//頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_;
	//indexリソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResource_;

	//データ****************************************************************************

	Material material_;
	bool isFileGltf_;

	//後に消すかも
	bool useMonsterBall = true;
public:
	
	static void PreDraw(ID3D12GraphicsCommandList* commandList);
	static void PreDrawParticle(ID3D12GraphicsCommandList* commandList);

	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	ModelData LoadModelGltf(const std::string& directoryPath, const std::string& filename);

	Node ReadNode(aiNode* node);
    /// <summary>
	/// モデル作成
	/// </summary>
	void CreateModel(const std::string& ModelName, const std::string& extension);
	
	/// <summary>
	/// マテリアルリソース
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// モデル描画
	/// </summary>
	void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ObjectColor& color, std::optional<uint32_t> textureHandle = std::nullopt);

	/// <summary>
	/// モデルバーティクル
	/// </summary>
	void DrawParticle( const uint32_t instanceNum,
		D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle, std::optional<uint32_t> textureHandle = std::nullopt);


	void DebugImGui();


	//getter
	ModelData GetModelData()const { return modelData_; }
	bool GetIsFileGltf()const { return isFileGltf_; }
};

