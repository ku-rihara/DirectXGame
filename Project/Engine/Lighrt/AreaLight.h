#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct AreaLightData {
    Vector4 color;
    Vector3 position;
    float padding1;
    Vector3 normal;
    float intensity;
    float width;
    float height;
    float decay;
    Vector3 up;
    float padding2;
};

/// <summary>
/// エリアライトクラス
/// </summary>
class AreaLight {
private:
    Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
    AreaLightData* lightData_;

public:
    AreaLight()  = default;
    ~AreaLight() = default;

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

    /// <summary>
    /// ライトコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="index">インデックス</param>
    void SetLightCommand(ID3D12GraphicsCommandList* commandList, const int& index);

    void DebugImGui(); //< ImGuiデバッグ表示
public:
    //getter
    ID3D12Resource* GetLightResource() const { return lightResource_.Get(); }
    AreaLightData* GetData() { return lightData_; }
};