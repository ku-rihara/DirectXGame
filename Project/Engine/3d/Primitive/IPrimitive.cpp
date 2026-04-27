#include "IPrimitive.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Pipeline/PipelineManager.h"
using namespace KetaEngine;

void IPrimitive::Init() {

    mesh_ = std::make_unique<Mesh>();
    CreateWVPResource();
    CreateMaterialResource();
    mesh_->Init(DirectXCommon::GetInstance(), vertexNum_);
    Create();
}

///============================================================
/// WVPリソース作成
///============================================================
void IPrimitive::CreateWVPResource() {
    wvpResource_ = DirectXCommon::GetInstance()->CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
    // データを書き込む
    wvpDate_ = nullptr;
    // 書き込むためのアドレスを取得
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
    // 単位行列を書き込んでおく
    wvpDate_->WVP                   = MakeIdentity4x4();
    wvpDate_->World                 = MakeIdentity4x4();
    wvpDate_->WorldInverseTranspose = MakeIdentity4x4();
}

///============================================================
/// 描画
///============================================================
void IPrimitive::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, std::optional<uint32_t> textureHandle) {
    if (!mesh_) {
        return;
    }

    auto* commandList = DirectXCommon::GetInstance()->GetCommandList();
    auto* pm          = PipelineManager::GetInstance();
    pm->PreDraw(PipelineType::Primitive, commandList);
    pm->PreBlendSet(PipelineType::Primitive, commandList, BlendMode::None);

    wvpDate_->World                 = worldTransform.matWorld_;
    wvpDate_->WVP                   = worldTransform.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;
    wvpDate_->WorldInverseTranspose = Inverse(Transpose(wvpDate_->World));

    auto* tm = TextureManager::GetInstance();
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
    if (textureHandle.has_value()) {
        gpuHandle = tm->GetTextureHandle(textureHandle.value());
    } else if (primitiveTextureHandle_ != 0) {
        gpuHandle = tm->GetTextureHandle(primitiveTextureHandle_);
    } else {
        gpuHandle = tm->GetNullTexture2DHandle();
    }
    mesh_->Draw(wvpResource_, material_, gpuHandle);
}

void IPrimitive::SetColor(const Vector4& color) {
    material_.SetColor(color);
}

void IPrimitive::SetTexture(const std::string& name) {
    currentTexturePath_ = name;
    primitiveTextureHandle_ = TextureManager::GetInstance()->LoadTexture(name);
    material_.GetMaterialData()->useTexture = 1;
}

void IPrimitive::RebuildInternal(uint32_t newVertexNum) {
    vertexNum_ = newVertexNum;
    mesh_      = std::make_unique<Mesh>();
    mesh_->Init(DirectXCommon::GetInstance(), newVertexNum);
    if (!currentTexturePath_.empty()) {
        primitiveTextureHandle_ = TextureManager::GetInstance()->LoadTexture(currentTexturePath_);
        material_.GetMaterialData()->useTexture = 1;
    }
}

void IPrimitive::CreateMaterialResource() {
    material_.Init(DirectXCommon::GetInstance());
}