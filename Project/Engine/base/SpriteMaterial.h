#pragma once
#include "Vector4.h"
#include"Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;

class SpriteMaterial {
private:
 
    struct MaterialSprite {
        Vector4 color;
        Matrix4x4 uvTransform;
       /* float padding[3];*/
    };

private:

    ///===================================
   ///private variant
   ///===================================

    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

public:

    ///===================================
    ///public variant
    ///===================================
  
    MaterialSprite* materialData_;
 
public:
 
    SpriteMaterial(); // コンストラクタ

   ///===================================
  ///public method
  ///===================================

    /// マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);
    /// マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color);
    /// シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

#ifdef _DEBUG
    // デバッグ用ImGuiでの表示
    void DebugImGui();
#endif

};
