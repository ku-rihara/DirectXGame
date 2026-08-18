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
/// スポットライトの色・位置・向き・減衰・照射角度を保持する定数バッファ用構造体
/// </summary>
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
    SpotLight() = default;
    ~SpotLight();

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

    void Update();     //< 更新処理
    void DebugImGui(); //< ImGuiデバッグ表示
    void AdjustParam(); //< パラメータ調整
    void RegisterParams(); //< パラメータ登録

private:
    bool isMove_ = false;
    Vector3 tempPos_;
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    SpotLightData* lightData_;
    GlobalParameter* globalParameter_;
    std::string groupName_;
    std::string sceneFolder_; 

    // デバッグ表示用
    std::unique_ptr<Line3D> debugLine_;

public:
    // getter
    SpotLightData* GetData() { return lightData_; }
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    const std::string& GetGroupName() const { return groupName_; }
};

}; // KetaEngine
