#pragma once

#include "Pipeline/BasePipeline.h"
#include <wrl.h>

namespace KetaEngine {

enum class ParticleDistortionRootParameter : UINT {
    ParticleData,    // t0 (VS) - インスタンシングデータ
    DistortionParam, // b0 (PS) - 歪み強度パラメータ（32ビット定数）
    DistortionMap,   // t0 (PS) - ノイズテクスチャ
    Count
};

/// <summary>
/// CPUパーティクル 歪みパス描画パイプライン
/// </summary>
class ParticleDistortionPipeline : public BasePipeline {
public:
    ParticleDistortionPipeline()  = default;
    ~ParticleDistortionPipeline() = default;

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
