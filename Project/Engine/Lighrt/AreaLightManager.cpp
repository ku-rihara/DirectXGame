#include "AreaLightManager.h"

void AreaLightManager::Add(ID3D12Device* device) {
    
    auto newLight = std::make_unique<AreaLight>();
    newLight->Init(device);
    areaLights_.push_back(std::move(newLight));
}

void AreaLightManager::Remove(int index) {
    if (index >= 0 && index < areaLights_.size()) {
        areaLights_.erase(areaLights_.begin() + index);
    }
}

std::vector<AreaLight*> AreaLightManager::GetLights() {
    std::vector<AreaLight*> lights;
    for (const auto& light : areaLights_){
        lights.push_back(light.get());
    }
    return lights;
}

void AreaLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    // 全てのスポットライトに対して処理を行う
    for (size_t i = 0; i < areaLights_.size(); ++i){
        // 各スポットライトのデータを設定
        areaLights_[i]->SetLightCommand(commandList,static_cast<int>(i));
    }
}