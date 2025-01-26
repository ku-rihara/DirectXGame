#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "Vector3.h"
#include "Vector4.h"

struct AreaLightData {
    Vector4 color;       // 16バイト
    Vector3 position;    // 12バイト
    float padding1;      // アライメント用パディング
    Vector3 normal;      // 12バイト
    float intensity;     // 4バイト
    float width;         // 4バイト
    float height;        // 4バイト
    float decay;         // 4バイト
    Vector3 up;          // 12バイト
    float padding2;      // アライメント用パディング
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


