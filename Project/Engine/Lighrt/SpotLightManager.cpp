#include "SpotLightManager.h"
#include"Dx/DirectXCommon.h"
#include<imgui.h>
#include<string>

void SpotLightManager::Add(ID3D12Device* device) {
    
    auto newLight = std::make_unique<SpotLight>();
    newLight->Init(device);
    spotLights_.push_back(std::move(newLight));
}

void SpotLightManager::Remove(int index) {
    if (index >= 0 && index < spotLights_.size()) {
        spotLights_.erase(spotLights_.begin() + index);
    }
}

std::vector<SpotLight*> SpotLightManager::GetLights() {
    std::vector<SpotLight*> lights;
    for (const auto& light : spotLights_){
        lights.push_back(light.get());
    }
    return lights;
}

void SpotLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    // 全てのスポットライトに対して処理を行う
    for (size_t i = 0; i < spotLights_.size(); ++i){
        // 各スポットライトのデータを設定
        spotLights_[i]->SetLightCommand(commandList,static_cast<int>(i));
    }
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
       
    }
}