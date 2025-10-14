#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

struct ConstBufferDataObjectColor {
    Vector4 color_;
};

// オブジェクト個別のカラー指定
class ObjectColor {
private:
    Vector4 color_ = Vector4::kWHITE();

    Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_; //< 定数バッファ
    ConstBufferDataObjectColor* constMap_ = nullptr; //<マッピング済みアドレス

    // 定数バッファの作成,マッピング
    void CreateConstBuffer();
    void Map();

public:
    void Init();
    void TransferMatrix();
    void SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, const UINT& rootParameterIndex) const;

    //Getter
    const Vector4& GetColor() const { return color_; }

    // Setter
    void SetColor(const Vector4& color) { color_ = color; }
};