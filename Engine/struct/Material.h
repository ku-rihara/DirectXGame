#pragma once
#include "Vector4.h"
#include"Matrix4x4.h"
#include <string>
#include <wrl.h>
#include <d3d12.h>

class DirectXCommon;

class Material {
public:
    // マテリアルの色情報
    Vector4 color;

    // マテリアルの反射率 (Shininess)
    float shininess;

    // UV座標変換用の行列
    Matrix4x4 uvTransform;

    // ライティングのモード（0: なし, 1: ランバート, 2: ハーフランバート, 3: スペキュラ）
    int enableLighting;

    // テクスチャファイルパス
    std::string textureFilePath;

    // コンストラクタ
    Material();

    // マテリアルのリソースを作成する関数
    void CreateMaterialResource(DirectXCommon* dxCommon);

    // マテリアルのデータを更新する関数
    void UpdateMaterialData(const Vector4& newColor);

    // シェーダーにデータを送る関数
    void SetToShader(ID3D12GraphicsCommandList* commandList);

#ifdef _DEBUG
    // デバッグ用ImGuiでの表示
    void DebugImGui();
#endif

private:
    // GPUリソースへのポインタ
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;

    // GPUに送るマテリアルデータの実体
    Material* materialData_;
};
