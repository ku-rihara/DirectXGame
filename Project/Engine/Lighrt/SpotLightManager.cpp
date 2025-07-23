#include "SpotLightManager.h"
#include "Dx/DirectXCommon.h"
#include "base/SrvManager.h"
#include <imgui.h>
#include <string>

void SpotLightManager::Init(ID3D12Device* device) {
    // 最大ライト数を仮定
    const size_t maxLights = 32;

    // リソースを作成
    structuredBufferResource_ = DirectXCommon::GetInstance()->CreateBufferResource(
        device, sizeof(SpotLightData) * maxLights);

    // マッピング
    structuredBufferData_ = nullptr;
    structuredBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&structuredBufferData_));

    // インデックスを取得
    srvIndex_ = SrvManager::GetInstance()->Allocate();

    // SRVを作成
    SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
        srvIndex_,
        structuredBufferResource_.Get(),
        maxLights,
        sizeof(SpotLightData));
}

void SpotLightManager::Update() {
    for (const auto& light : spotLights_) {
        light->Update();
    }
}

void SpotLightManager::Add(ID3D12Device* device, const int32_t number) {
    auto newLight = std::make_unique<SpotLight>();

    newLight->Init(device, groupName_ +std::to_string(number).c_str());
    spotLights_.push_back(std::move(newLight));
   
    UpdateStructuredBuffer();
}

void SpotLightManager::Remove(int index) {
    if (index >= 0 && index < spotLights_.size()) {
        spotLights_.erase(spotLights_.begin() + index);

        UpdateStructuredBuffer();
    }
}

void SpotLightManager::UpdateStructuredBuffer() {
    // 全てのライトデータをStructured Bufferにコピー
    for (size_t i = 0; i < spotLights_.size(); ++i) {
        structuredBufferData_[i] = *spotLights_[i]->GetData();
    }
}

std::vector<SpotLight*> SpotLightManager::GetLights() {
    std::vector<SpotLight*> lights;
    for (const auto& light : spotLights_) {
        lights.push_back(light.get());
    }
    return lights;
}

void SpotLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    // Structured Bufferを更新
    UpdateStructuredBuffer();

    // SRVを設定
    commandList->SetGraphicsRootDescriptorTable(7,SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_));
}

void SpotLightManager::AdJustParams() {
    if (ImGui::CollapsingHeader("SpotLights")) {
        const auto& spotLights = GetLights();
        for (size_t i = 0; i < spotLights.size(); ++i) {
            if (ImGui::TreeNode(("SpotLight" + std::to_string(i)).c_str())) {
                spotLights[i]->AdjustParam();
                ImGui::TreePop();
            }
        }
    }
}