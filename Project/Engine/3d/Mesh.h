#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
// data
#include "struct/VertexData.h"

#include "Material/MeshMaterial.h"
#include"Material/ParticleMaterial.h"
#include <cstdint>
#include <optional>

class DirectXCommon;
class Mesh {
public:
    Mesh()  = default;
    ~Mesh() = default;

    ///==========================================================
    /// public method
    ///==========================================================

    void Init(DirectXCommon* directXCommon, const uint32_t& vertexNum);
    void CreateVertexResource();
    void SetIndexData(const uint32_t* indices, const uint32_t& indexCount);
    void DrawInstancing(const uint32_t& instanceNum,const D3D12_GPU_DESCRIPTOR_HANDLE& instancingGUPHandle, BaseMaterial* material, const std::optional<uint32_t>& textureHandle);
    void DebugImGui();

    void Draw(Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource, MeshMaterial material, const std::optional<uint32_t>& textureHandle = std::nullopt);
   
    void SetTexture(const std::string& name);

private:
    ///==========================================================
    /// protected variable
    ///==========================================================

    DirectXCommon* directXCommon_ = nullptr;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
    uint32_t indexNum_;

    // Resource data
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; // 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_; // indexリソース

    // Transform データ
    VertexData* vertexDate_ = nullptr;

    // 頂点数
    uint32_t vertexNum_;

    uint32_t textureHandle_;

public:
    ///==========================================================
    /// getter method
    ///==========================================================

    ///==========================================================
    /// setter method
    ///==========================================================

    // vertexData
    void SetVertexPositionData(const uint32_t& index, const Vector4& pos) { vertexDate_[index].position = pos; }
    void SetVertexNormData(const uint32_t& index, const Vector3& norm) { vertexDate_[index].normal = norm; }
    void SetVertexTexcoordData(const uint32_t& index, const Vector2& texcoord) { vertexDate_[index].texcoord = texcoord; }
};
