#pragma once
#include <vector>
#include<memory>
#include "PointLight.h"

class PointLightManager {
private:
    std::vector<std::unique_ptr<PointLight>> pointLights_;
public:
    void Add(ID3D12Device* device);
    void Remove(int index);
    std::vector<PointLight*> GetLights();
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
};