#pragma once
#include "utility/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

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

/// <summary>
/// スポットライトクラス
/// </summary>
class SpotLight {
public:
    SpotLight()  = default;
    ~SpotLight() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="groupName">グループ名</param>
    void Init(ID3D12Device* device, const std::string& groupName);

    /// <summary>
    /// ライトの位置を設定
    /// </summary>
    /// <param name="pos">位置</param>
    void SetPosition(const Vector3& pos);

    void Update();     //< 更新処理
    void DebugImGui(); //< ImGuiデバッグ表示
    void AdjustParam(); //< パラメータ調整
    void BindParams(); //< パラメータバインド

private:
    bool isMove_;
    Vector3 tempPos_;
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    SpotLightData* lightData_;
    GlobalParameter* globalParameter_;
    std::string groupName_;

public:
    // getter
    SpotLightData* GetData() { return lightData_; }
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
};