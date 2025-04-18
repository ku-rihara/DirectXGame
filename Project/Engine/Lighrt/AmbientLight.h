#pragma once
#include <wrl.h>
#include <d3d12.h>
#include"Vector3.h"
#include"Vector4.h"

struct AmbientLightData {
    Vector4 color;      
    float intensity;    
    float padding[3];      
};
class AmbientLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    AmbientLightData* lightData_;

public:
    AmbientLight();
    ~AmbientLight();

    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }


    void SetColor(const Vector4& color);
    void SetIntensity(float intensity);
    void DebugImGui();

    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
  
};