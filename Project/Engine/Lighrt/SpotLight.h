#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct SpotLightData {
    Vector4 color; // ライトの色
    Vector3 position; // ライトの位置
    float intensity; // 輝度
    Vector3 direction; // スポットライトの方向
    float distance; // ライトの届く最大距離
    float decay; // 減衰率
    float cosAngle; // スポットライトの余弦
    float cosFalloffStart; // 減衰開始の余弦
    float padding[2];
};

class SpotLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    SpotLightData* lightData_;

public:
    SpotLight()  = default;
    ~SpotLight() = default;
    void SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index);

    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    void SetPosition(const Vector3& pos);
    void DebugImGui();
    SpotLightData* GetData() { return lightData_; }
};