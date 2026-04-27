#include "SkyBox.h"
#include "Base/Dx/DirectXCommon.h"
#include "Base/TextureManager.h"
#include "Pipeline/PipelineManager.h"
#include "math/MathFunction.h"
#include <imgui.h>

using namespace KetaEngine;

SkyBox::SkyBox() {}
SkyBox::~SkyBox() {}

void SkyBox::Init() {
    primitiveBox_ = std::make_unique<PrimitiveBox>();
    primitiveBox_->Init();

    auto* dxCommon = DirectXCommon::GetInstance();

    // SkyBox専用WVPリソース
    wvpResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(TransformationMatrix));
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpDate_));
    wvpDate_->WVP                   = MakeIdentity4x4();
    wvpDate_->World                 = MakeIdentity4x4();
    wvpDate_->WorldInverseTranspose = MakeIdentity4x4();

    // SkyBox専用マテリアル
    skyBoxMaterial_.Init(dxCommon);

    // キューブマップテクスチャをロード
    textureHandle_ = TextureManager::GetInstance()->LoadTexture("Resources/Texture/output.dds");

    transform_.Init();
    transform_.scale_ = {300.0f, 300.0f, 300.0f};
}

void SkyBox::Update() {
    transform_.UpdateMatrix();
}

void SkyBox::Draw(ViewProjection& viewProjection) {
    auto* commandList = DirectXCommon::GetInstance()->GetCommandList();
    auto* pm          = PipelineManager::GetInstance();

    // SkyBox専用パイプライン（TextureCube対応、ブレンドなし）
    pm->PreDraw(PipelineType::SkyBox, commandList);

    // WVP更新
    wvpDate_->World = transform_.matWorld_;
    wvpDate_->WVP   = transform_.matWorld_ * viewProjection.matView_ * viewProjection.matProjection_;

    // TextureCube SRVを直接バインド
    D3D12_GPU_DESCRIPTOR_HANDLE texHandle = TextureManager::GetInstance()->GetTextureHandle(textureHandle_);
    primitiveBox_->GetMesh()->Draw(wvpResource_, skyBoxMaterial_, texHandle);
}

void SkyBox::Debug() {
#if defined(_DEBUG) || defined(DEVELOPMENT)
    if (ImGui::CollapsingHeader("SkyBox")) {
        ImGui::PushID("SkyBox");
        ImGui::DragFloat3("Position", &transform_.translation_.x, 0.1f);
        ImGui::DragFloat3("Scale", &transform_.scale_.x, 0.1f);
        ImGui::PopID();
    }
#endif
}
