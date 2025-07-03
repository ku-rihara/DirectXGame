#include "Line3D.h"
#include "base/Line3DCommon.h"
#include "base/Object3dCommon.h"
#include "Dx/DirectXCommon.h"
#include <cassert>
#include <cstring>
#include <d3dx12.h>

// 初期化
void Line3D::Init() {
    auto dxCommon = DirectXCommon::GetInstance();
    HRESULT hr;

    kMaxVertices_ = kMaxLines_ * kVerticesPerLine_;
    // 頂点配列の確保
    vertices_.resize(kMaxVertices_);

    UINT bufferSize = static_cast<UINT>(sizeof(Vertex) * vertices_.size());

    D3D12_HEAP_PROPERTIES heapProps = {D3D12_HEAP_TYPE_UPLOAD};
    D3D12_RESOURCE_DESC vbDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    hr = dxCommon->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_));
    assert(SUCCEEDED(hr));

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes    = static_cast<UINT>(bufferSize);
    vertexBufferView_.StrideInBytes  = sizeof(Vertex);

    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    constantBufferResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(CBuffer));
    cBufferData_            = nullptr;
    constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&cBufferData_));
    cBufferData_->viewProjection = MakeIdentity4x4();

    Reset();
}

void Line3D::SetLine(const Vector3& start, const Vector3& end, const Vector4& color) {
    if (currentLineCount_ >= kMaxLines_) {
        return;
    }

    vertices_[currentLineCount_ * 2]     = {start, color};
    vertices_[currentLineCount_ * 2 + 1] = {end, color};
    currentLineCount_++;
}

// 描画
void Line3D::Draw(ID3D12GraphicsCommandList* commandList, const ViewProjection& viewProj) {
    if (currentLineCount_ == 0) {
        return;
    }

    Line3DCommon::GetInstance()->PreDraw(commandList);

    // 転送
    std::memcpy(vertexData_, vertices_.data(), sizeof(Vertex) * currentLineCount_ * kVerticesPerLine_);
    *cBufferData_ = {viewProj.matView_ * viewProj.matProjection_};

    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->SetGraphicsRootConstantBufferView(0, constantBufferResource_->GetGPUVirtualAddress());
    commandList->DrawInstanced(static_cast<UINT>(currentLineCount_ * kVerticesPerLine_), 1, 0, 0);

    Reset();

    Object3DCommon::GetInstance()->PreDraw(commandList);
}

void Line3D::Reset() {
    currentLineCount_ = 0;
}