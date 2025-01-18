#pragma once
#include <vector>
#include "PointLight.h"

class PointLightManager {
private:
    std::vector<PointLight> pointLights_;
public:
    void Add(ID3D12Device* device);
    void Remove(int index);
    std::vector<PointLight>& GetLights();
 
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
};