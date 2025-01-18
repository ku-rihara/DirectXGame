#include "SpotLightManager.h"

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
    for (const auto& light : spotLights_)
    {
        lights.push_back(light.get());
    }
    return lights;
}

void SpotLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
   
        spotLights_[0]->SetLightCommand(commandList);
    
}