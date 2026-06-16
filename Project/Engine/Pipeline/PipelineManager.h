#pragma once

#include "Base/Material/ModelMaterial.h"
#include <array>
#include <d3d12.h>
#include <memory>

namespace KetaEngine {

class DirectXCommon;
class BasePipeline;
class SpritePipeline;
class Object3DPipeline;
class SkinningObject3DPipeline;
class ParticlePipeline;
class GPUParticlePipeline;
class ShadowMapPipeline;
class Line3DPipeline;
class SkyBoxPipeline;
class RibbonTrailPipeline;
class DistortionPipeline;
class PrimitivePipeline;

enum class PipelineType {
    Sprite,
    Object3D,
    SkinningObject3D,
    Particle,
    GPUParticle,
    ShadowMap,
    Line3D,
    SkyBox,
    RibbonTrail,
    DistortionRibbon,
    DistortionParticle,
    Primitive,
    Count
};

/// <summary>
/// グラフィックスパイプライン管理クラス
/// </summary>
class PipelineManager {
public:
    static PipelineManager* GetInstance();

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="dxCommon">DirectXCommon</param>
    void Init(DirectXCommon* dxCommon);

    /// <summary>
    /// 描画前処理
    /// </summary>
    /// <param name="type">パイプラインタイプ</param>
    /// <param name="commandList">コマンドリスト</param>
    void PreDraw(const PipelineType& type, ID3D12GraphicsCommandList* commandList) const;

    /// <summary>
    /// ブレンドモード設定
    /// </summary>
    /// <param name="type">パイプラインタイプ</param>
    /// <param name="commandList">コマンドリスト</param>
    /// <param name="blendMode">ブレンドモード</param>
    void PreBlendSet(const PipelineType& type, ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) const;

    void Finalize(); //< 終了処理

private:
    PipelineManager()  = default;
    ~PipelineManager() = default;
    // コピー禁止
    PipelineManager(const PipelineManager&)            = delete;
    PipelineManager& operator=(const PipelineManager&) = delete;
    PipelineManager(PipelineManager&&)                 = delete;
    PipelineManager& operator=(PipelineManager&&)      = delete;

private:
    std::array<std::unique_ptr<BasePipeline>, static_cast<size_t>(PipelineType::Count)> pipelines_;
    DirectXCommon* dxCommon_ = nullptr;

public:
    BasePipeline* GetPipeline(const PipelineType& type) const;
};

}; // KetaEngine
