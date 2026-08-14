#pragma once
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace KetaEngine {

/// <summary>
/// ポイントライト1つ分のGPU送信用データ(色・位置・輝度・半径・減衰率)
/// </summary>
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

    // GlobalParameter
    GlobalParameter* globalParameter_;
    std::string groupName_;

public:
    PointLight()  = default;
    ~PointLight() = default;

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

    void RegisterParams(); //< パラメータバインド
    void DebugImGui(); //< ImGuiデバッグ表示
public:
     //getter
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    PointLightData* GetData() { return lightData_; }
};

}; // KetaEngine
