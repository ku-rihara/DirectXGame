#pragma once

#include<wrl.h>
#include<d3d12.h>


#include <optional>
#include<string>

#include<assimp/scene.h>
//struct
#include"struct/ModelData.h"

#include"base/Material.h"
#include"base/DirectXCommon.h"

//3Dモデル共通部
class ModelCommon {
private:
	///============================================================
    /// private variants
    ///============================================================	
	DirectXCommon* dxCommon_;
public:
	///============================================================
    /// public method
    ///============================================================
	void Init(DirectXCommon* dxCommon);
	///============================================================
	/// getter method
	///============================================================
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
};

class TextureManager;
class Model {
private:
	
	///============================================================
	/// private variants
	///============================================================	

	///テクスチャ
	TextureManager* textureManager_ = nullptr;
	uint32_t textureHandle_;
	ModelData modelData_;
	
	/// GPUHandle,BufferView
	D3D12_GPU_DESCRIPTOR_HANDLE handle_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	/// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>vertexResource_; //頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource>indexResource_; //indexリソース

	bool isFileGltf_;// gitfファイルかのフラグ

public:

	///============================================================
    /// public method
    ///============================================================	
	
	/// モデル作成
	void CreateModel(const std::string& ModelName, const std::string& extension);

	/// モデルロード
	ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
	ModelData LoadModelGltf(const std::string& directoryPath, const std::string& filename);

	/// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);


	Node ReadNode(aiNode* node);
	void DebugImGui();// デバッグ表示

	///============================================================
	/// Draw method
	///============================================================	

	void Draw(
		Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, Material material,
		std::optional<uint32_t> textureHandle = std::nullopt);	/// モデル描画
	
	void DrawParticle(
		const uint32_t instanceNum,D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle, 
		Material material,
		std::optional<uint32_t> textureHandle = std::nullopt);/// モデルバーティクル

	///============================================================
	/// getter method
	///============================================================	
	ModelData GetModelData()const { return modelData_; }
	bool GetIsFileGltf()const { return isFileGltf_; }
};

