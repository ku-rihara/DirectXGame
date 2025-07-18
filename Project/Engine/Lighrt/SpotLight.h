#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct SpotLightData {
    Vector4 color; 
    Vector3 position; 
    float intensity; 
    Vector3 direction; 
    float distance;
    float decay; 
    float cosAngle; 
    float cosFalloffStart; 
};

class SpotLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    SpotLightData* lightData_;

public:
    SpotLight()  = default;
    ~SpotLight() = default;
  
    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    void SetPosition(const Vector3& pos);
    void DebugImGui();
    SpotLightData* GetData() { return lightData_; }
};