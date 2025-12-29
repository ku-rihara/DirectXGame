#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

namespace KetaEngine {

struct AmbientLightData {
    Vector4 color;
    float intensity;
    float padding[3];
};

/// <summary>
/// 環境光ライトクラス
/// </summary>
class AmbientLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    AmbientLightData* lightData_;

public:
    AmbientLight()  = default;
    ~AmbientLight() = default;

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="device">デバイス</param>
    void Init(ID3D12Device* device);

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
};

}; // KetaEngine
