#pragma once
#include <wrl.h>
#include <d3d12.h>
#include"Vector3.h"
#include"Vector4.h"

struct AmbientLightData {
    Vector4 color;       // 16 バイト
    float intensity;     // 4 バイト
    float radius;        // 4 バイト
    float decay;         // 4 バイト
    float padding;       // 4 バイト（アライメント調整用）
    Vector3 position;    // 12 バイト
    float padding2;      // 4 バイト（アライメント調整用）
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