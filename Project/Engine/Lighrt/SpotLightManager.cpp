#include "SpotLightManager.h"

void SpotLightManager::Add(ID3D12Device* device) {
    SpotLight newLight;
    newLight.Init(device);
    spotLights_.push_back(newLight);
}

void SpotLightManager::Remove(int index) {
    if (index >= 0 && index < spotLights_.size()) {
        spotLights_.erase(spotLights_.begin() + index);
    }
}

std::vector<SpotLight>& SpotLightManager::GetLights() {
    return spotLights_;
}

void SpotLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    spotLights_[0].SetLightCommand(commandList);
}