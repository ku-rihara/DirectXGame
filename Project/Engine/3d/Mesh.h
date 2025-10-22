#pragma once
#include "Material/MeshMaterial.h"
#include "Material/ParticleMaterial.h"
#include "struct/VertexData.h"
#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <optional>
#include <wrl.h>

class DirectXCommon;

/// <summary>
/// メッシュクラス
/// </summary>
class Mesh {
public:
    Mesh()  = default;
    ~Mesh() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="directXCommon">DirectXCommonのインスタンス</param>
    /// <param name="vertexNum">頂点数</param>
    void Init(DirectXCommon* directXCommon, const uint32_t& vertexNum);

    /// <summary>
    /// インデックスデータを設定
    /// </summary>
    /// <param name="indices">インデックス配列</param>
    /// <param name="indexCount">インデックス数</param>
    void SetIndexData(const uint32_t* indices, const uint32_t& indexCount);

    /// <summary>
    /// インスタンシング描画
    /// </summary>
    /// <param name="instanceNum">インスタンス数</param>
    void DrawInstancing(const uint32_t& instanceNum);

    void CreateVertexResource(); //< 頂点リソース作成
    void DebugImGui();           //< ImGuiデバッグ表示

    /// <summary>
    /// 描画
    /// </summary>
    /// <param name="wvpResource">WVPリソース</param>
    /// <param name="material">マテリアル</param>
    /// <param name="textureHandle">テクスチャハンドル(オプション)</param>
    void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, MeshMaterial material, const std::optional<uint32_t>& textureHandle = std::nullopt);

    /// <summary>
    /// テクスチャを設定
    /// </summary>
    /// <param name="name">テクスチャ名</param>
    void SetTexture(const std::string& name);

private:
    DirectXCommon* directXCommon_ = nullptr;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_; //< 頂点バッファビュー
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{}; //< インデックスバッファビュー
    uint32_t indexNum_; //< インデックス数

    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; //< 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_; //< インデックスリソース

    VertexData* vertexDate_ = nullptr; //< 頂点データ

    uint32_t vertexNum_;     //< 頂点数
    uint32_t textureHandle_; //< テクスチャハンドル

public:
    void SetVertexPositionData(const uint32_t& index, const Vector4& pos) { vertexDate_[index].position = pos; }
    void SetVertexNormData(const uint32_t& index, const Vector3& norm) { vertexDate_[index].normal = norm; }
    void SetVertexTexcoordData(const uint32_t& index, const Vector2& texcoord) { vertexDate_[index].texcoord = texcoord; }
};
