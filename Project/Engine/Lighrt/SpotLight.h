#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include"utility/ParameterEditor/GlobalParameter.h"
#include <d3d12.h>
#include <wrl.h>
#include <string>

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
public:
    SpotLight()  = default;
    ~SpotLight() = default;

    void Init(ID3D12Device* device,const std::string&groupName);
    void Update();
    void DebugImGui();
   

    ///-------------------------------------------------------------------------------------
    /// Editor
    ///-------------------------------------------------------------------------------------
    void AdjustParam();
    void BindParams();

private:
    bool isMove_;
    Vector3 tempPos_;
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    SpotLightData* lightData_;
    GlobalParameter* globalParameter_;
    std::string groupName_;

public:
    SpotLightData* GetData() { return lightData_; }
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    void SetPosition(const Vector3& pos);
};