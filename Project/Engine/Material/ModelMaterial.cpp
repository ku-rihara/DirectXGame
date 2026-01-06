#include "ModelMaterial.h"

using namespace KetaEngine;
#include "base/TextureManager.h"
#include "Dx/DirectXCommon.h"
#include "Pipeline/Object3D/Object3DPipeline.h"
#include <cassert>
#include <imgui.h>


void ModelMaterial::Init(DirectXCommon* dxCommon) {
    assert(dxCommon);

    // マテリアルリソース作成
    materialResource_ = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(MaterialData));

    // マテリアルデータへのポインタ取得
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // 初期値をセット
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); 
    materialData_->shininess = 12.5f;
    materialData_->uvTransform = MakeIdentity4x4();
    materialData_->enableLighting = 2;
    materialData_->environmentCoefficient = 0.0f;

    // Dissolve用の初期値
    materialData_->dissolveThreshold = 1.0f;
    materialData_->dissolveEdgeColor = Vector3(1.0f, 0.5f, 0.0f);
    materialData_->dissolveEdgeWidth = 0.03f;
    materialData_->enableDissolve    = 0;

     // DissolvePlayer初期化
    dissolvePlayer_ = std::make_unique<DissolvePlayer>();
    dissolvePlayer_->Init();

    dissolveTextureIndex_ = TextureManager::GetInstance()->LoadTexture("Resources/EngineTexture/Noise/noise0.png");
}

void ModelMaterial::UpdateMaterialData(const Vector4& Color) {
    materialData_->color = Color;
}

void ModelMaterial::SetCommandList(ID3D12GraphicsCommandList* commandList) {
    // シェーダーにマテリアルデータを送る
    commandList->SetGraphicsRootConstantBufferView(static_cast<UINT>(Object3DRootParameter::Material), materialResource_->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(Object3DRootParameter::Dissolve), TextureManager::GetInstance()->GetTextureHandle(dissolveTextureIndex_));
}

void ModelMaterial::UpdateDissolve(float speedRate) {
    if (dissolvePlayer_) {
        dissolvePlayer_->Update(speedRate);

        // 自動的にマテリアルに適用
        if (dissolvePlayer_->IsPlaying()) {
            dissolvePlayer_->ApplyToMaterial(*this);
        }
    }
}

void ModelMaterial::PlayDissolve(const std::string& dissolveName) {
    if (dissolvePlayer_) {
        dissolvePlayer_->Play(dissolveName);
    }
}

bool ModelMaterial::IsDissolveFinished() const {
    return dissolvePlayer_ ? dissolvePlayer_->IsFinished() : true;
}

bool ModelMaterial::IsDissolvePlaying() const {
    return dissolvePlayer_ ? dissolvePlayer_->IsPlaying() : false;
}

void ModelMaterial::DebugImGui() {
#ifdef _DEBUG
    ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&materialData_->color));
    ImGui::DragFloat("Shininess", &materialData_->shininess, 0.01f);
    ImGui::DragFloat("environmentCoefficient", &materialData_->environmentCoefficient, 0.01f);
    const char* lightingModes[] = {"No Lighting", "Lambert", "Half Lambert", "Specular Reflection",
        "PointLight", "SpotLight", "AreaLight", "Ambient"};
    ImGui::Combo("Lighting Mode", &materialData_->enableLighting, lightingModes, IM_ARRAYSIZE(lightingModes));

    ImGui::Separator();
    ImGui::Text("Dissolve Settings");

    bool enableDissolve = (materialData_->enableDissolve != 0);
    if (ImGui::Checkbox("Enable Dissolve", &enableDissolve)) {
        materialData_->enableDissolve = enableDissolve ? 1 : 0;
    }

    if (enableDissolve) {
        ImGui::SliderFloat("Dissolve Threshold", &materialData_->dissolveThreshold, 0.0f, 1.0f);
        ImGui::ColorEdit3("Dissolve Edge Color", reinterpret_cast<float*>(&materialData_->dissolveEdgeColor));
        ImGui::SliderFloat("Dissolve Edge Width", &materialData_->dissolveEdgeWidth, 0.01f, 0.1f);
    }

#endif
}

void ModelMaterial::SetDissolveNoizeTexture(const std::string& name) {
    dissolveTextureIndex_ = TextureManager::GetInstance()->LoadTexture(name);
}
