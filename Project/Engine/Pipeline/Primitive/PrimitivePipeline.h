#pragma once
#include "../BasePipeline.h"
#include "Base/Dx/DirectXCommon.h"
#include <wrl/client.h>

namespace KetaEngine {

enum class PrimitiveRootParameter : UINT {
    Material,           ///< b0 Pixel: PrimitiveMaterial CB
    TransformationMatrix, ///< b0 Vertex: WVP CB
    Texture2D,          ///< t0 Pixel: テクスチャ
    Count
};

/// <summary>
/// プリミティブ描画パイプライン
/// </summary>
class PrimitivePipeline : public BasePipeline {
public:
    PrimitivePipeline()  = default;
    ~PrimitivePipeline() = default;

    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    void CreateRootSignature() override;
    void CreateGraphicsPipeline() override;

private:
    D3D12_STATIC_SAMPLER_DESC staticSampler_{};

    Microsoft::WRL::ComPtr<ID3D12PipelineState> psoAdd_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> psoNone_;
};

}; // KetaEngine
