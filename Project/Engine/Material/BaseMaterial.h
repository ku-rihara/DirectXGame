#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector4.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

class DirectXCommon;

/// <summary>
/// マテリアルの基底クラス
/// </summary>
class BaseMaterial {
public:
    BaseMaterial()          = default;
    virtual ~BaseMaterial() = default;

    /// <summary>
    /// マテリアルリソースの作成
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    virtual void CreateMaterialResource(DirectXCommon* dxCommon) = 0;

    /// <summary>
    /// マテリアルデータの更新
    /// </summary>
    /// <param name="Color">色</param>
    virtual void UpdateMaterialData(const Vector4& Color) = 0;

    /// <summary>
    /// コマンドリストへの設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    virtual void SetCommandList(ID3D12GraphicsCommandList* commandList) = 0;

    virtual void DebugImGui() = 0; //< ImGuiデバッグ

protected:
    // 共通リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:
    virtual void SetShininess(float shininess)                           = 0;
    virtual void SetEnvironmentCoefficient(float environmentCoefficient) = 0;
};