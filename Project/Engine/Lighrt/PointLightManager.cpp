#include "PointLightManager.h"

void PointLightManager::Add(ID3D12Device* device) {
    PointLight newLight;
    newLight.Init(device);
    pointLights_.push_back(newLight);
}

void PointLightManager::Remove(int index) {
    if (index >= 0 && index < pointLights_.size()) {
        pointLights_.erase(pointLights_.begin() + index);
    }
}

std::vector<PointLight>& PointLightManager::GetLights() {
    return pointLights_;
}

void PointLightManager::SetLightCommand(ID3D12GraphicsCommandList* commandList) {
    pointLights_[0].SetLightCommand(commandList);
}