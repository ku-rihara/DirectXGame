#pragma once

#include "Pipeline/BasePipeline.h"
#include <wrl.h>

namespace KetaEngine {

/// どちらの歪みモードで初期化するか
enum class DistortionMode { Ribbon, Particle };

/// RibbonTrail 歪みパス用ルートパラメータ
enum class DistortionRibbonRootParam : UINT {
    TransformationMatrix, // b0 (VS) - ビュープロジェクション行列
    DistortionUVScroll,   // b1 (VS) - 歪みテクスチャUVスクロール
    DistortionParam,      // b0 (PS) - 歪み強度
    DistortionMap,        // t0 (PS) - ノイズ/法線テクスチャ
    Count
};

/// CPUParticle 歪みパス用ルートパラメータ
enum class DistortionParticleRootParam : UINT {
    ParticleData,    // t0 (VS) - インスタンシングデータ
    DistortionParam, // b0 (PS) - 歪み強度
    DistortionMap,   // t0 (PS) - ノイズテクスチャ
    Count
};

/// <summary>
/// 歪みパス描画パイプライン
/// </summary>
class DistortionPipeline : public BasePipeline {
public:
    explicit DistortionPipeline(DistortionMode mode);
    ~DistortionPipeline() = default;

    void Init(DirectXCommon* dxCommon) override;
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) override;

protected:
    void CreateRootSignature() override;
    void CreateGraphicsPipeline() override;

private:
    void CreateRibbonRootSignature();
    void CreateParticleRootSignature();
    void CreateRibbonPipeline();
    void CreateParticlePipeline();

    DistortionMode mode_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso_;
};

} // namespace KetaEngine
