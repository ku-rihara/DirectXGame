#pragma once
#include "Vector4.h"
#include"Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;

class SpriteMaterial {
private:
    // スプライト用構造体
    struct MaterialSprite {
        Vector4 color;
        Matrix4x4 uvTransform;
       /* float padding[3];*/
    };
public:
    // GPUに送るマテリアルデータの実体(Sprite)
    MaterialSprite* materialData_;
 
private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:

    // コンストラクタ
    SpriteMaterial();

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color);

    // シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList);



#ifdef _DEBUG
    // デバッグ用ImGuiでの表示
    void DebugImGui();
#endif

};
