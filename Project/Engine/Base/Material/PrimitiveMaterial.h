#pragma once
#include "BaseMaterial.h"
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

namespace KetaEngine {

class DirectXCommon;

/// <summary>
/// プリミティブ用マテリアルクラス
/// </summary>
class PrimitiveMaterial : public BaseMaterial {
public:
    PrimitiveMaterial()  = default;
    ~PrimitiveMaterial() = default;

    void Init(DirectXCommon* dxCommon) override;
    void UpdateMaterialData(const Vector4& Color) override;
    void SetCommandList(ID3D12GraphicsCommandList* commandList) override;
    void DebugImGui() override;

private:
    struct MaterialData {
        Vector4  color          = {1.0f, 1.0f, 1.0f, 1.0f};
        int32_t  useTexture     = 0;       ///< 1=テクスチャサンプリング有効
        float    angleMaskRatio = 1.0f;    ///< 角度マスク比率 0〜1 (1=全体表示)
        int32_t  pad[2]         = {0, 0};  ///< 16byte アライメント
    };

    MaterialData* materialData_ = nullptr;

public:
    MaterialData* GetMaterialData() { return materialData_; }

    void SetColor(const Vector4& color) { materialData_->color = color; }
};

}; // KetaEngine
