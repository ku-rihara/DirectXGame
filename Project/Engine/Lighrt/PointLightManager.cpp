#include "PointLightManager.h"

void PointLightManager::Add(ID3D12Device* device) {
    auto newLight = std::make_unique<PointLight>();
    newLight->Init(device);
    pointLights_.push_back(std::move(newLight));
}

void PointLightManager::Remove(int index) {
    if (index >= 0 && index < pointLights_.size()) {
        pointLights_.erase(pointLights_.begin() + index);
    }
}
std::vector<PointLight*> PointLightManager::GetLights() {
    std::vector<PointLight*> lights;
    for (const auto& light : pointLights_)
    {
        lights.push_back(light.get());
    }
    return lights;
}

void PointLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
  
        pointLights_[0]->SetLightCommand(commandList);
    
}