#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct PointLightData {
    Vector4 color; // ライトの色
    Vector3 position; // ライトの位置
    float intenesity; // 輝度
    float radius; // ライトの幅
    float decay; // 減衰率
};

/// <summary>
/// ポイントライトクラス
/// </summary>
class PointLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    PointLightData* lightData_;

public:
    PointLight()  = default;
    ~PointLight() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    void Init(ID3D12Device* device);

    /// <summary>
    /// ライトの位置を設定
    /// </summary>
    /// <param name="pos">位置</param>
    void SetPosition(const Vector3& pos);

    void DebugImGui(); //< ImGuiデバッグ表示
public:
     //getter
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    PointLightData* GetData() { return lightData_; }
};