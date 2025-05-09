#pragma once
#include "Vector4.h"
#include"Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;

class Material {
private:
    struct MaterialStructure {
        Vector4 color;
        int32_t enableLighting;
        float padding[3];
        Matrix4x4 uvMatrix;
        float shininess;
    };
   
public:
    // GPUに送るマテリアルデータの実体
    MaterialStructure* materialData_;
   
private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;


public:

    // コンストラクタ
    Material();

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& Color);

    // シェーダーにデータを送る関数
    void SetCommandList(ID3D12GraphicsCommandList* commandList);

    void DebugImGui();


};
