#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Vector3.h"
#include "Vector4.h"

struct AreaLightData {
    Vector4 color;           // ライトの色
    Vector3 position;        // ライトの中心位置
    float intensity;         // 輝度
    Vector3 normal;          // エリアライトの法線
    float radius;            // エリアライトのサイズ(x,y,zでそれぞれ幅、高さ、奥行き)
    float decay;             // 減衰率
    float padding[3];
};

class AreaLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    AreaLightData* lightData_;

public:
    AreaLight();
    ~AreaLight();
    void SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index);

    void Init(ID3D12Device* device);
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    void SetPosition(const Vector3& pos);
    void DebugImGui();
    AreaLightData* GetData() { return lightData_; }
};