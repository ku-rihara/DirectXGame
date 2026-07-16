#pragma once

#include "Pipeline/BasePipeline.h"
#include <wrl.h>

namespace KetaEngine {

/// どちらの歪みモードで初期化するか
enum class DistortionMode { Ribbon, Particle };

/// RibbonTrail 歪みパス用ルートパラメータ
enum class DistortionRibbonRootParam : UINT {
    TransformationMatrix, //  ビュープロジェクション行列
    DistortionUVScroll,   //  歪みテクスチャUVスクロール
    DistortionParam,      //  歪み強度
    DistortionMap,        //  ノイズ/法線テクスチャ
    Count
};

/// CPUParticle 歪みパス用ルートパラメータ
enum class DistortionParticleRootParam : UINT {
    ParticleData,    //  インスタンシングデータ
    DistortionParam, //  歪み強度
    DistortionMap,   //  ノイズテクスチャ
    Count
};

/// <summary>
/// 歪みパス描画パイプライン
/// </summary>
class DistortionPipeline : public BasePipeline {
public:
    DistortionPipeline(DistortionMode mode);
    ~DistortionPipeline() = default;

    void Init(DirectXCommon* dxCommon) override;

    // 描画前処理、ブレンド設定処理
    void PreDraw(ID3D12GraphicsCommandList* commandList) override;
    void PreBlendSet(ID3D12GraphicsCommandList*, const BlendMode&) override;

protected:

    // ルートシグネチャ作成、グラフィックスパイプライン作成
    void CreateRootSignature() override;
    void CreateGraphicsPipeline() override;

private:

    // ルートシグネチャ作成、グラフィックスパイプライン作成
    void CreateRibbonRootSignature();
    void CreateParticleRootSignature();
    void CreateRibbonPipeline();
    void CreateParticlePipeline();

    DistortionMode mode_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso_;
};

} // namespace KetaEngine
