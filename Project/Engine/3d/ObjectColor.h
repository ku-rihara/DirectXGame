#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

namespace KetaEngine {

struct ConstBufferDataObjectColor {
    Vector4 color_;
};

/// <summary>
/// オブジェクトのカラー指定クラス
/// </summary>
class ObjectColor {
public:
    void Init();//< 初期化
    
    /// <summary>
    /// グラフィックスコマンドを設定
    /// </summary>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="rootParameterIndex">ルートパラメータインデックス</param>
    void SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, const UINT& rootParameterIndex) const;

    void TransferMatrix(); //< 行列転送
    void CreateConstBuffer(); //< 定数バッファ作成
    void Map();               //< マッピング

private:
    Vector4 color_ = Vector4::kWHITE();

    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
    ConstBufferDataObjectColor* constMap_ = nullptr;

public:
    const Vector4& GetColor() const { return color_; }
    void SetColor(const Vector4& color) { color_ = color; }
};

}; // KetaEngine
