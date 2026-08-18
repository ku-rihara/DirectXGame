#pragma once
#include "3D/Line3D/Line3D.h"
#include "Editor/ParameterEditor/GlobalParameter.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <memory>
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
    std::string sceneFolder_; 

    // デバッグ表示用
    std::unique_ptr<Line3D> debugLine_;

public:
    PointLight() = default;
    ~PointLight();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    /// <param name="groupName">グループ名</param>
    /// <param name="sceneFolder">保存・読込先のシーン別フォルダ名</param>
    void Init(ID3D12Device* device, const std::string& groupName, const std::string& sceneFolder);

    /// <summary>
    /// ライトの位置を設定
    /// </summary>
    /// <param name="pos">位置</param>
    void SetPosition(const Vector3& pos);

    void Update(); //< 更新処理
    void RegisterParams(); //< パラメータバインド
    void DebugImGui(); //< ImGuiデバッグ表示
public:
     //getter
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    PointLightData* GetData() { return lightData_; }
    const std::string& GetGroupName() const { return groupName_; }
};

}; // KetaEngine
