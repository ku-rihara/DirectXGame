#pragma once

#include <d3d12.h>
#include <wrl.h>

#include <optional>
#include <string>

#include <assimp/scene.h>
// struct
#include "struct/ModelData.h"

#include "Animation/SkinCluster.h"

#include "Material/Material.h"
#include "Dx/DirectXCommon.h"
#include "Material/ParticleMaterial.h"

// 3Dモデル共通部
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
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
};

class TextureManager;
class ShadowMap;

class Model {

	 

public:
    ///============================================================
    /// public method
    ///============================================================
    Model() = default;
    ~Model();
    /// モデル作成
    void CreateModel(const std::string& ModelFileName);

    /// モデルロード
    ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);
    ModelData LoadModelGltf(const std::string& directoryPath, const std::string& filename);

    /// 描画前処理
    static void PreDraw(ID3D12GraphicsCommandList* commandList);

    Node ReadNode(aiNode* node);
    void DebugImGui();
    void Finalize();

    ///============================================================
    /// Draw method
    ///============================================================

    void Draw(
        Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ShadowMap&shadowMap, Material material,
        std::optional<uint32_t> textureHandle = std::nullopt); /// モデル描画

    void DrawAnimation(
        Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ShadowMap& shadowMap, Material material,
        SkinCluster skinCluster, std::optional<uint32_t> textureHandle = std::nullopt); /// モデル描画

    void DrawInstancing(
        const uint32_t instanceNum, D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle,
        ParticleMaterial material,
        std::optional<uint32_t> textureHandle = std::nullopt); /// モデルバーティクル

private:
    ///============================================================
    /// private variants
    ///============================================================

    /// テクスチャ
    TextureManager* textureManager_ = nullptr;
    DirectXCommon* dxCommon_        = nullptr;

    uint32_t textureHandle_;
    ModelData modelData_;

    /// GPUHandle,BufferView
    D3D12_GPU_DESCRIPTOR_HANDLE handle_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    /// リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; // 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_; // indexリソース

    bool isFileGltf_; // gltfファイルかのフラグ

    std::string modelPath_   = "Resources/Model/";
    std::string texturePath_ = "Resources/Texture/";

public:
    ///============================================================
    /// getter method
    ///============================================================
    ModelData GetModelData() { return modelData_; }
    bool GetIsFileGltf() const { return isFileGltf_; }
};
