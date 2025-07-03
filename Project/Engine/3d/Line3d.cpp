#include "Line3D.h"
#include "base/Line3DCommon.h"
#include "base/Object3dCommon.h"
#include "Dx/DirectXCommon.h"
#include <cassert>
#include <cstring>
#include <d3dx12.h>
#include <cmath>
#include <numbers>
#include <cstdint>

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

void Line3D::DrawSphereWireframe(const Vector3& center, float radius, const Vector4& color) {
    const int32_t latMax = 8;
    const int32_t lonMax = 8;

    using std::numbers::pi_v;
    const float PI = pi_v<float>;

    // 緯度方向（Y軸を中心に）+Zから-Zへ
    for (int32_t lat = 0; lat <= latMax; ++lat) {
        float theta1 = PI * (float)(lat) / (float)latMax;
        float theta2 = PI * (float)(lat + 1) / (float)latMax;

        for (int32_t lon = 0; lon < lonMax; ++lon) {
            float phi1 = 2.0f * PI * (float)(lon) / (float)lonMax;
            float phi2 = 2.0f * PI * (float)(lon + 1) / (float)lonMax;

            Vector3 p1 = {
                radius * std::sin(theta1) * std::cos(phi1),
                radius * std::cos(theta1),
                radius * std::sin(theta1) * std::sin(phi1),
            };
            Vector3 p2 = {
                radius * std::sin(theta1) * std::cos(phi2),
                radius * std::cos(theta1),
                radius * std::sin(theta1) * std::sin(phi2),
            };
            Vector3 p3 = {
                radius * std::sin(theta2) * std::cos(phi1),
                radius * std::cos(theta2),
                radius * std::sin(theta2) * std::sin(phi1),
            };

            // 中心にオフセットして線追加
            SetLine(center + p1, center + p2, color);
            SetLine(center + p1, center + p3, color);
        }
    }
}


void Line3D::Reset() {
    currentLineCount_ = 0;
}