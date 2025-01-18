#pragma once
#include <vector>
#include "SpotLight.h"

class SpotLightManager {
private:
    std::vector<SpotLight> spotLights_;
public:
    void Add(ID3D12Device* device);
    void Remove(int index);
    std::vector<SpotLight>& GetLights();
   
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
};