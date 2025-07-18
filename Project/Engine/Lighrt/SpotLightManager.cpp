#include "SpotLightManager.h"
#include "Dx/DirectXCommon.h"
#include "base/SrvManager.h"
#include <imgui.h>
#include <string>

void SpotLightManager::Init(ID3D12Device* device) {
    // 最大ライト数を仮定（必要に応じて調整）
    const uint32_t maxLights = 32;

    // Structured Buffer用のリソースを作成
    structuredBufferResource_ = DirectXCommon::GetInstance()->CreateBufferResource(
        device, sizeof(SpotLightData) * maxLights);

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
        sizeof(SpotLightData));
}

void SpotLightManager::Add(ID3D12Device* device) {
    auto newLight = std::make_unique<SpotLight>();
    newLight->Init(device);
    spotLights_.push_back(std::move(newLight));

    // Structured Bufferを更新
    UpdateStructuredBuffer();
}

void SpotLightManager::Remove(int index) {
    if (index >= 0 && index < spotLights_.size()) {
        spotLights_.erase(spotLights_.begin() + index);

        // Structured Bufferを更新
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

    // SRVを設定（ルートパラメータのインデックスは適切に調整してください）
    commandList->SetGraphicsRootDescriptorTable(
        7, // ルートパラメータのインデックス
        SrvManager::GetInstance()->GetGPUDescriptorHandle(srvIndex_));
}

void SpotLightManager::DebugImGui() {
    if (ImGui::CollapsingHeader("SpotLights")) {
        const auto& spotLights = GetLights();
        for (size_t i = 0; i < spotLights.size(); ++i) {
            if (ImGui::TreeNode(("SpotLight" + std::to_string(i)).c_str())) {
                spotLights[i]->DebugImGui();
                ImGui::TreePop();
            }
        }
        if (ImGui::Button("Add Spot Light")) {
            Add(DirectXCommon::GetInstance()->GetDevice());
        }

        // ライト数の表示
        ImGui::Text("Light Count: %zu", spotLights.size());
    }
}