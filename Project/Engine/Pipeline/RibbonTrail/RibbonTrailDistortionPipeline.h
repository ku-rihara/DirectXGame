#pragma once

#include "Pipeline/BasePipeline.h"
#include <wrl.h>

namespace KetaEngine {

enum class RibbonTrailDistortionRootParameter : UINT {
    TransformationMatrix, // b0 (VS) - ビュープロジェクション行列
    DistortionParam,      // b1 (PS) - 歪み強度パラメータ
    DistortionMap,        // t0 (PS) - ノイズ/法線テクスチャ
    Count
};

/// <summary>
/// リボントレイル 時空歪みパス描画パイプライン
/// </summary>
class RibbonTrailDistortionPipeline : public BasePipeline {
public:
    RibbonTrailDistortionPipeline()  = default;
    ~RibbonTrailDistortionPipeline() = default;

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
