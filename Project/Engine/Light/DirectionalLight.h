#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

namespace KetaEngine {

struct DirectionalLightData {
    Vector4 color; // ライトの色
    Vector3 direction; // ライトの向き
    float intensity; // 輝度
};

/// <summary>
/// ディレクショナルライトクラス
/// </summary>
class DirectionalLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    DirectionalLightData* lightData_;

public:
    DirectionalLight()  = default;
    ~DirectionalLight() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    void Init(ID3D12Device* device);

    /// <summary>
    /// ライトの向きを設定
    /// </summary>
    /// <param name="direction">向き</param>
    void SetDirection(const Vector3& direction);

    /// <summary>
    /// ライトの色を設定
    /// </summary>
    /// <param name="color">色</param>
    void SetColor(const Vector4& color);

    /// <summary>
    /// ライトの強度を設定
    /// </summary>
    /// <param name="intensity">強度</param>
    void SetIntensity(float intensity);

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    void SetLightCommand(ID3D12GraphicsCommandList* commandList);

    void DebugImGui(); //< ImGuiデバッグ表示
public:
    // getter
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    Vector3 GetDirection() const { return lightData_->direction; }
};

}; // KetaEngine
