#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>
#include "Matrix4x4.h"
#include <cstdint>



// 定数バッファ用データ構造体
struct ConstBufferDataMaterial {
    Vector4 color_;
    int32_t enableLighting;
    float padding[3];
    Matrix4x4 uvTransform;
    float shininess;
};

// マテリアル情報
class Material {
private:
    Vector4 color_ = { 1, 1, 1, 1 };
    int32_t enableLighting_ = 1;
    Matrix4x4 uvTransform_;
    float shininess_ = 0.5f;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    // マッピング済みアドレス
    ConstBufferDataMaterial* constMap_ = nullptr;

    /// <summary>
    /// 定数バッファ生成
    /// </summary>
    void CreateConstBuffer();

    /// <summary>
    /// マッピングする
    /// </summary>
    void Map();

public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Init();

    /// <summary>
    /// マテリアルデータを転送する
    /// </summary>
    void TransferData();

    /// <summary>
    /// グラフィックスコマンドを積む
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="rootParameterIndex">ルートパラメータインデックス</param>
    void SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) const;

    /// <summary>
    /// getter
    /// </summary>
    Vector4 GetColor() const { return color_; }
    int32_t GetEnableLighting() const { return enableLighting_; }
    Matrix4x4 GetUVTransform() const { return uvTransform_; }
    float GetShininess() const { return shininess_; }

    /// <summary>
    /// setter
    /// </summary>
    void SetColor(const Vector4& color) { color_ = color; }
    void SetEnableLighting(int32_t enableLighting) { enableLighting_ = enableLighting; }
    void SetUVTransform(const Matrix4x4& uvTransform) { uvTransform_ = uvTransform; }
    void SetShininess(float shininess) { shininess_ = shininess; }
};
