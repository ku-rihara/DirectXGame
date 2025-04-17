#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
// data
#include "struct/TransformationMatrix.h"
#include "struct/VertexData.h"

#include "base/Material.h"
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
    void DrawInstancing(const uint32_t instanceNum, D3D12_GPU_DESCRIPTOR_HANDLE instancingGUPHandle, Material material, std::optional<uint32_t> textureHandle);
    void DebugImGui();

private:
    ///==========================================================
    /// pritected variable
    ///==========================================================

    DirectXCommon* directXCommon_ = nullptr;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    // Resource data
    Material material_;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_; // 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_; // wvpリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_; // indexリソース

    // Transform データ
    TransformationMatrix* wvpDate_;
    VertexData* vertexDate_ = nullptr;

    // 頂点数
    uint32_t vertexNum_;

    uint32_t defaultTextureHandle_;

public:
    ///==========================================================
    /// getter method
    ///==========================================================

    ///==========================================================
    /// setter method
    ///==========================================================
    void SetwvpDate(Matrix4x4 date) { this->wvpDate_->WVP = date; }
    void SetWorldMatrixDate(Matrix4x4 date) { wvpDate_->World = date; }

    // vertexData
    void SetVertexPositionData(const uint32_t index, const Vector4& pos) { vertexDate_[index].position = pos; }
    void SetVertexNormData(const uint32_t index, const Vector3& norm) { vertexDate_[index].normal = norm; }
    void SetVertexTexcoordData(const uint32_t index, const Vector2& texcoord) { vertexDate_[index].texcoord = texcoord; }
};
