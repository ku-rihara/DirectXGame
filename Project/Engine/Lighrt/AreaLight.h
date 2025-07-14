#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Vector3.h"
#include "Vector4.h"

struct AreaLightData {
    Vector4 color;      
    Vector3 position;   
    float padding1;     
    Vector3 normal;     
    float intensity;    
    float width;        
    float height;       
    float decay;        
    Vector3 up;         
    float padding2;     
};


class AreaLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    AreaLightData* lightData_;

public:
    AreaLight()=default;
    ~AreaLight() = default;
    void SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index);

    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    void SetPosition(const Vector3& pos);
    void DebugImGui();
    AreaLightData* GetData() { return lightData_; }
};
