#pragma once
#include "Vector4.h"
#include"Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// スプライトのマテリアルクラス
/// </summary>
class SpriteMaterial {
private:
    struct MaterialSprite {
        Vector4 color;
        Matrix4x4 uvTransform;
    };

public:
    SpriteMaterial();
    ~SpriteMaterial()=default;
 
    /// <summary>
    /// マテリアルリソースの作成
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void CreateMaterialResource(DirectXCommon* dxCommon);

    /// <summary>
    /// マテリアルのデータの更新
    /// </summary>
    /// <param name="Color">色</param>
    void UpdateMaterialData(const Vector4& Color);

    /// <summary>
    /// シェーダーにデータを送る
    /// </summary>
    /// <param name="commandList"></param>
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

    void DebugImGui(); //< ImGuiデバッグ表示

private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:
    MaterialSprite* materialData_;

};

}; // KetaEngine
