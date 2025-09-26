#include "PointLightManager.h"
#include "Dx/DirectXCommon.h"
#include "base/SrvManager.h"
#include <imgui.h>
#include <string>

void PointLightManager::Init(ID3D12Device* device) {

    // 最大ライト数
    const size_t maxLights = 32;

    structuredBufferResource_ = DirectXCommon::GetInstance()->CreateBufferResource(
        device, sizeof(PointLightData) * maxLights);

    // マッピング
    structuredBufferData_ = nullptr;
    structuredBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&structuredBufferData_));

    // インデックスを取得
   srvIndex_ = SrvManager::GetInstance()->Allocate();

    //SRVを作成
    SrvManager::GetInstance()->CreateForStructuredBuffer(
        srvIndex_,
        structuredBufferResource_.Get(),
        maxLights,
        sizeof(PointLightData));
}

void PointLightManager::Add(ID3D12Device* device) {
    auto newLight = std::make_unique<PointLight>();
    newLight->Init(device);
    pointLights_.push_back(std::move(newLight));

    UpdateStructuredBuffer();
}

void PointLightManager::Remove(int index) {
    if (index >= 0 && index < pointLights_.size()) {
        pointLights_.erase(pointLights_.begin() + index);

        UpdateStructuredBuffer();
    }
}

void PointLightManager::UpdateStructuredBuffer() {
    // 全てのライトデータをStructured Bufferにコピー
    for (size_t i = 0; i < pointLights_.size(); ++i) {
        structuredBufferData_[i] = *pointLights_[i]->GetData();
    }
}

std::vector<PointLight*> PointLightManager::GetLights() {
    std::vector<PointLight*> lights;
    for (const auto& light : pointLights_) {
        lights.push_back(light.get());
    }
    return lights;
}

void PointLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    // Structured Bufferを更新
    UpdateStructuredBuffer();

    // SRVを設定
    commandList->SetGraphicsRootDescriptorTable(6, SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_));
}

void PointLightManager::DebugImGui() {
    if (ImGui::CollapsingHeader("PointLights")) {
        const auto& pointLights = GetLights();
        for (size_t i = 0; i < pointLights.size(); ++i) {
            if (ImGui::TreeNode(("PointLight" + std::to_string(i)).c_str())) {
                pointLights[i]->DebugImGui();
                ImGui::TreePop();
            }
        }
       
    }
}