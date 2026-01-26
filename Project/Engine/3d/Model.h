#pragma once
#include "3D/AnimationObject3D/SkinCluster.h"
#include "Base/Material/ModelMaterial.h"
#include "struct/ModelData.h"
#include <assimp/scene.h>
#include <d3d12.h>
#include <optional>
#include <string>
#include <wrl.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// 3Dモデル共通部
/// </summary>
class ModelCommon {
private:
    DirectXCommon* dxCommon_;

public:
    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommonのインスタンス</param>
    void Init(DirectXCommon* dxCommon);

public:
    DirectXCommon* GetDxCommon() const { return dxCommon_; }
};

class TextureManager;
class ShadowMap;
class BaseMaterial;

/// <summary>
/// 3Dモデルクラス
/// </summary>
class Model {
public:
    Model() = default;
    ~Model();

    /// <summary>
    /// モデル作成
    /// </summary>
    /// <param name="ModelFileName">モデルファイル名</param>
    void CreateModel(const std::string& ModelFileName);

    /// <summary>
    /// OBJファイルからモデルデータを読み込む
    /// </summary>
    /// <param name="directoryPath">ディレクトリパス</param>
    /// <param name="filename">ファイル名</param>
    /// <returns>モデルデータ</returns>
    ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

    /// <summary>
    /// GLTFファイルからモデルデータを読み込む
    /// </summary>
    /// <param name="directoryPath">ディレクトリパス</param>
    /// <param name="filename">ファイル名</param>
    /// <returns>モデルデータ</returns>
    ModelData LoadModelGltf(const std::string& directoryPath, const std::string& filename);

    /// <summary>
    /// ノード情報を読み込む
    /// </summary>
    /// <param name="node">assimpノード</param>
    /// <returns>ノード</returns>
    Node ReadNode(aiNode* node);

    void DebugImGui(); //< ImGuiデバッグ表示
    void Finalize();   //< 終了処理

    /// <summary>
    /// モデル描画
    /// </summary>
    /// <param name="wvpResource">WVPリソース</param>
    /// <param name="shadowMap">シャドウマップ</param>
    /// <param name="material">マテリアル</param>
    /// <param name="textureHandle">テクスチャハンドル(オプション)</param>
    void Draw(
        Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ShadowMap& shadowMap, ModelMaterial* material,
        const std::optional<uint32_t>& textureHandle = std::nullopt);

    /// <summary>
    /// アニメーションモデル描画
    /// </summary>
    /// <param name="wvpResource">WVPリソース</param>
    /// <param name="shadowMap">シャドウマップ</param>
    /// <param name="material">マテリアル</param>
    /// <param name="skinCluster">スキンクラスター</param>
    /// <param name="textureHandle">テクスチャハンドル(オプション)</param>
    void DrawAnimation(
        Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ShadowMap& shadowMap, ModelMaterial* material, const SkinCluster& skinCluster,
        const std::optional<uint32_t>& textureHandle = std::nullopt);

    /// <summary>
    /// インスタンシング描画
    /// </summary>
    /// <param name="instanceNum">インスタンス数</param>
    void DrawInstancing(uint32_t instanceNum);

    /// <summary>
    /// シャドウマップ用描画
    /// </summary>
    /// <param name="wvpResource">WVPリソース</param>
    /// <param name="shadowMap">シャドウマップ</param>
    void DrawForShadowMap(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, const ShadowMap& shadowMap);

private:
    TextureManager* textureManager_ = nullptr;
    DirectXCommon* dxCommon_        = nullptr;

    uint32_t textureIndex_;
    ModelData modelData_;

    D3D12_GPU_DESCRIPTOR_HANDLE handle_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;

    bool isFileGltf_; //< GLTFファイルかのフラグ

    std::string modelPath_   = "Resources/Model/";
    std::string texturePath_ = "Resources/Texture/";
    std::string modelName_   = "";

public:
    uint32_t GetTextureIndex() const { return textureIndex_; }
    const ModelData& GetModelData() { return modelData_; }
    const std::string& GetModelName() const { return modelName_; }
    ID3D12Resource* GetVertexResource() { return vertexResource_.Get(); }
    bool GetIsFileGltf() const { return isFileGltf_; }
};

}; // KetaEngine
