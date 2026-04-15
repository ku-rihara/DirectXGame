#include "RibbonTrail.h"
#include "RibbonTrailManager.h"

using namespace KetaEngine;
#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Pipeline/RibbonTrail/RibbonTrailPipeline.h"
#include "Pipeline/Distortion/DistortionPipeline.h"
#include <cassert>
#include <cmath>
#include <d3dx12.h>

///============================================================
/// デストラクタ
///============================================================
RibbonTrail::~RibbonTrail() {
    if (RibbonTrailManager::GetInstance()) {
        RibbonTrailManager::GetInstance()->Unregister(this);
    }
}

///============================================================
/// RibbonTrailオブジェクト作成
///============================================================
RibbonTrail* RibbonTrail::Create(size_t maxPoints) {
    std::unique_ptr<RibbonTrail> trail = std::make_unique<RibbonTrail>();
    trail->Init(maxPoints);
    RibbonTrailManager::GetInstance()->Register(trail.get());
    return trail.release();
}

///============================================================
/// 初期化
///============================================================
void RibbonTrail::Init(size_t maxPoints) {
    maxPoints_ = maxPoints;

    // 頂点バッファは maxPoints * 2
    UINT bufferSize = static_cast<UINT>(sizeof(RibbonVertex) * maxPoints * 2);
    auto dxCommon   = DirectXCommon::GetInstance();

    D3D12_HEAP_PROPERTIES heapProps = {D3D12_HEAP_TYPE_UPLOAD};
    D3D12_RESOURCE_DESC   vbDesc    = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

   HRESULT hr = dxCommon->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &vbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_));

    if (FAILED(hr)) {
       
        throw std::runtime_error("CreateCommittedResource failed");
    }

    vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes    = bufferSize;
    vertexBufferView_.StrideInBytes  = sizeof(RibbonVertex);

    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    constantBufferResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(RibbonCBuffer));
    constantBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&cBufferData_));
    cBufferData_->viewProjection = MakeIdentity4x4();

    // UVスクロール定数バッファ（メイン）
    uvScrollCBufferResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(RibbonUVScrollCBuffer));
    uvScrollCBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&uvScrollCBufferData_));
    uvScrollCBufferData_->offset = Vector2::ZeroVector();

    // UVスクロール定数バッファ（歪み）
    distortionUVScrollCBufferResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(RibbonUVScrollCBuffer));
    distortionUVScrollCBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&distortionUVScrollCBufferData_));
    distortionUVScrollCBufferData_->offset = Vector2::ZeroVector();

    // デフォルトテクスチャをロード
    SetTexture("");
}

///============================================================
/// テクスチャ設定
///============================================================
void RibbonTrail::SetTexture(const std::string& texturePath) {
    const std::string& path = texturePath.empty() ? kDefaultTexturePath : texturePath;
    textureHandle_ = TextureManager::GetInstance()->LoadTexture(path);
}

///============================================================
/// 歪みテクスチャ設定
///============================================================
void RibbonTrail::SetDistortionTexture(const std::string& texturePath) {
    if (texturePath.empty()) {
        distortionTextureHandle_ = UINT32_MAX;
    } else {
        distortionTextureHandle_ = TextureManager::GetInstance()->LoadTexture(texturePath);
    }
}

///============================================================
/// ポイント追加
///============================================================
void RibbonTrail::AddPoint(const Vector3& position, const Vector4& startColor, float startWidth, float lifetime) {
    if (points_.size() >= maxPoints_) {
        points_.pop_back();
    }
    points_.push_front({position, startColor, startWidth, lifetime, 0.0f});
}

///============================================================
/// 更新（期限切れポイント削除）
///============================================================
void RibbonTrail::Update(float deltaTime) {
    for (auto& p : points_) {
        p.age += deltaTime;
    }
    while (!points_.empty() && points_.back().age >= points_.back().lifetime) {
        points_.pop_back();
    }

    // UVオフセット累積（小数部のみ保持してオーバーフロー防止）
    uvScrollOffset_.x = std::fmod(uvScrollOffset_.x + uvScrollSpeed_.x * deltaTime, 1.0f);
    uvScrollOffset_.y = std::fmod(uvScrollOffset_.y + uvScrollSpeed_.y * deltaTime, 1.0f);

    distortionUVScrollOffset_.x = std::fmod(distortionUVScrollOffset_.x + distortionUVScrollSpeed_.x * deltaTime, 1.0f);
    distortionUVScrollOffset_.y = std::fmod(distortionUVScrollOffset_.y + distortionUVScrollSpeed_.y * deltaTime, 1.0f);
}

///============================================================
/// 幅方向ベクトル計算（ビルボード対応）
///============================================================
Vector3 RibbonTrail::CalcPerp(const Vector3& dir, const Vector3& cameraRight) {
    // トレイル進行方向 × カメラ右ベクトル = トレイル幅方向
    Vector3 perp = {
        dir.y * cameraRight.z - dir.z * cameraRight.y,
        dir.z * cameraRight.x - dir.x * cameraRight.z,
        dir.x * cameraRight.y - dir.y * cameraRight.x};
    float len = std::sqrt(perp.x * perp.x + perp.y * perp.y + perp.z * perp.z);

    if (len < 0.001f) {
        // フォールバック: カメラ右ベクトルをそのまま使用
        return cameraRight;
    }

    return {perp.x / len, perp.y / len, perp.z / len};
}

///============================================================
/// 描画
///============================================================
void RibbonTrail::Draw(const ViewProjection& viewProj) {
    size_t count = points_.size();
    if (count < 2) {
        lastVertexCount_ = 0; 
        return;
    }

    // カメラ右ベクトル取得（ビュー行列0列目）
    Vector3 cameraRight = {
        viewProj.matView_.m[0][0],
        viewProj.matView_.m[1][0],
        viewProj.matView_.m[2][0]};
    float crLen = std::sqrt(
        cameraRight.x * cameraRight.x +
        cameraRight.y * cameraRight.y +
        cameraRight.z * cameraRight.z);
    if (crLen > 0.001f) {
        cameraRight = {cameraRight.x / crLen, cameraRight.y / crLen, cameraRight.z / crLen};
    }

    size_t vertexCount = 0;
    float  countF      = static_cast<float>(count > 1 ? count - 1 : 1);

    // ねじれ防止用：前セグメントのperp
    Vector3 prevPerp = {0.0f, 0.0f, 0.0f}; 

    for (size_t i = 0; i < count; ++i) {
        const auto& p = points_[i];

        // 移動方向
        Vector3 dir;
        if (i == 0) {
            dir = {p.position.x - points_[1].position.x,
                   p.position.y - points_[1].position.y,
                   p.position.z - points_[1].position.z};
        } else if (i == count - 1) {
            dir = {points_[i - 1].position.x - p.position.x,
                   points_[i - 1].position.y - p.position.y,
                   points_[i - 1].position.z - p.position.z};
        } else {
            dir = {points_[i - 1].position.x - points_[i + 1].position.x,
                   points_[i - 1].position.y - points_[i + 1].position.y,
                   points_[i - 1].position.z - points_[i + 1].position.z};
        }

        float dirLen = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
        if (dirLen > 0.001f) {
            dir = {dir.x / dirLen, dir.y / dirLen, dir.z / dirLen};
        }

        Vector3 perp = CalcPerp(dir, cameraRight);

        // ねじれ防止：前セグメントと逆向きならflip
        if (i > 0) {
            float dot = prevPerp.x * perp.x + prevPerp.y * perp.y + prevPerp.z * perp.z;
            if (dot < 0.0f) {
                perp = {-perp.x, -perp.y, -perp.z};
            }
        }
        prevPerp = perp;

        float t = static_cast<float>(i) / countF;

        float   aliveRate = 1.0f - (p.age / p.lifetime);
        Vector4 lerpColor = {
            Lerp(p.startColor.x, endColor_.x, t),
            Lerp(p.startColor.y, endColor_.y, t),
            Lerp(p.startColor.z, endColor_.z, t),
            Lerp(p.startColor.w, endColor_.w, t) * aliveRate};

        float hw = Lerp(p.startWidth, endWidth_, t) * 0.5f;
        float u  = t;

        vertexData_[vertexCount++] = {
            {p.position.x + perp.x * hw, p.position.y + perp.y * hw, p.position.z + perp.z * hw},
            lerpColor,
            {u, 0.0f}};
        vertexData_[vertexCount++] = {
            {p.position.x - perp.x * hw, p.position.y - perp.y * hw, p.position.z - perp.z * hw},
            lerpColor,
            {u, 1.0f}};
    }

    lastVertexCount_ = vertexCount;

    if (vertexCount < 4) {
        return;
    }

    cBufferData_->viewProjection    = viewProj.matView_ * viewProj.matProjection_;
    uvScrollCBufferData_->offset    = uvScrollOffset_;

    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->SetGraphicsRootConstantBufferView(
        static_cast<UINT>(RibbonTrailRootParameter::TransformationMatrix),
        constantBufferResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(
        static_cast<UINT>(RibbonTrailRootParameter::UVScroll),
        uvScrollCBufferResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(
        static_cast<UINT>(RibbonTrailRootParameter::Texture),
        TextureManager::GetInstance()->GetTextureHandle(textureHandle_));
    commandList->DrawInstanced(static_cast<UINT>(vertexCount), 1, 0, 0);
}

///============================================================
/// 時空歪みパス描画
///============================================================
void RibbonTrail::DrawDistortion(const ViewProjection& viewProj) {
    if (!useDistortion_) {
        return;
    }
    if (lastVertexCount_ < 4) {
        return;
    }
    if (distortionTextureHandle_ == UINT32_MAX) {
        return;
    }

    cBufferData_->viewProjection           = viewProj.matView_ * viewProj.matProjection_;
    distortionUVScrollCBufferData_->offset = distortionUVScrollOffset_;

    float distortionParams[4] = {distortionStrength_, 0.0f, 0.0f, 0.0f};

    auto commandList = DirectXCommon::GetInstance()->GetCommandList();
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    commandList->SetGraphicsRootConstantBufferView(
        static_cast<UINT>(DistortionRibbonRootParam::TransformationMatrix),
        constantBufferResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(
        static_cast<UINT>(DistortionRibbonRootParam::DistortionUVScroll),
        distortionUVScrollCBufferResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRoot32BitConstants(
        static_cast<UINT>(DistortionRibbonRootParam::DistortionParam),
        4, distortionParams, 0);
    commandList->SetGraphicsRootDescriptorTable(
        static_cast<UINT>(DistortionRibbonRootParam::DistortionMap),
        TextureManager::GetInstance()->GetTextureHandle(distortionTextureHandle_));
    commandList->DrawInstanced(static_cast<UINT>(lastVertexCount_), 1, 0, 0);
}

///============================================================
/// クリア
///============================================================
void RibbonTrail::Clear() {
    points_.clear();
}
