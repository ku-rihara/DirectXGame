#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct DirectionalLightData {
    Vector4 color; // ライトの色
    Vector3 direction; // ライトの向き
    float intensity; // 輝度
};

class DirectionalLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    DirectionalLightData* lightData_;

public:
    DirectionalLight()  = default;
    ~DirectionalLight() = default;

    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    Vector3 GetDirection() const { return lightData_->direction; }

    void SetDirection(const Vector3& direction);
    void SetColor(const Vector4& color);
    void SetIntensity(float intensity);
    void DebugImGui();

    void SetLightCommand(ID3D12GraphicsCommandList* commandList);
};