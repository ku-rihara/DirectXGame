#pragma once
#include <wrl.h>
#include <d3d12.h>
#include"Vector3.h"
#include"Vector4.h"

struct PointLightData {
    Vector4 color;    // ライトの色
    Vector3 position; // ライトの位置
    float intenesity; // 輝度
    float radius;     // ライトの幅
    float decay;      // 減衰率
};

class PointLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    PointLightData* lightData_;

public:
    PointLight()=default;
    ~PointLight() = default;

    void Init(ID3D12Device* device);
   
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }

    void SetPosition(const Vector3& pos);
    void DebugImGui();
    PointLightData* GetData() { return lightData_; }
 
};