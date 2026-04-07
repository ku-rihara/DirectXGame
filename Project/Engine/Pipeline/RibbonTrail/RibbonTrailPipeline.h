#pragma once

#include "Pipeline/BasePipeline.h"
#include <wrl.h>

namespace KetaEngine {

enum class RibbonTrailRootParameter : UINT {
    TransformationMatrix, // b0 (VS) - ビュープロジェクション行列
    UVScroll,             // b1 (VS) - UVスクロールオフセット
    Texture,              // t0 (PS) - テクスチャ
    Count
};

/// <summary>
/// リボントレイル描画パイプライン
/// </summary>
class RibbonTrailPipeline : public BasePipeline {
public:
    RibbonTrailPipeline()  = default;
    ~RibbonTrailPipeline() = default;

    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    void CreateRootSignature() override;
    void CreateGraphicsPipeline() override;

private:
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};

} // namespace KetaEngine
