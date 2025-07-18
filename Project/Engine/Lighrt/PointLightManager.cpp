#include "PointLightManager.h"
#include "Dx/DirectXCommon.h"
#include "base/SrvManager.h"
#include <imgui.h>
#include <string>

void PointLightManager::Init(ID3D12Device* device) {
    // 最大ライト数を仮定（必要に応じて調整）
    const uint32_t maxLights = 32;

    // Structured Buffer用のリソースを作成
    structuredBufferResource_ = DirectXCommon::GetInstance()->CreateBufferResource(
        device, sizeof(PointLightData) * maxLights);

    // マッピング
    structuredBufferData_ = nullptr;
    structuredBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&structuredBufferData_));

    // SRVManagerからインデックスを取得
   srvIndex_ = SrvManager::GetInstance()->Allocate();

    // Structured Buffer用のSRVを作成
    SrvManager::GetInstance()->CreateSRVforStructuredBuffer(
        srvIndex_,
        structuredBufferResource_.Get(),
        maxLights,
        sizeof(PointLightData));
}

void PointLightManager::Add(ID3D12Device* device) {
    auto newLight = std::make_unique<PointLight>();
    newLight->Init(device);
    pointLights_.push_back(std::move(newLight));

    // Structured Bufferを更新
    UpdateStructuredBuffer();
}

void PointLightManager::Remove(int index) {
    if (index >= 0 && index < pointLights_.size()) {
        pointLights_.erase(pointLights_.begin() + index);

        // Structured Bufferを更新
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
        if (ImGui::Button("Add Point Light")) {
            Add(DirectXCommon::GetInstance()->GetDevice());
        }

        // ライト数の表示
        ImGui::Text("Light Count: %zu", pointLights.size());
    }
}