#include "PipelineManager.h"

// Pipeline includes
#include "Pipeline/Line3D/Line3DPipeline.h"
#include "Pipeline/Object3D/Object3DPipeline.h"
#include "Pipeline/Particle/GPUParticlePipeline.h"
#include "Pipeline/Particle/ParticlePipeline.h"
#include "Pipeline/ShadowMap/ShadowMapPipeline.h"
#include "Pipeline/Skinning/SkinningObject3DPipeline.h"
#include "Pipeline/Sprite/SpritePipeline.h"

#include "Dx/DirectXCommon.h"
#include <cassert>

PipelineManager* PipelineManager::GetInstance() {
    static PipelineManager instance;
    return &instance;
}

void PipelineManager::Init(DirectXCommon* dxCommon) {
    assert(dxCommon);

    dxCommon_ = dxCommon;

    // 生成
    pipelines_[static_cast<size_t>(PipelineType::Sprite)]           = std::make_unique<SpritePipeline>();
    pipelines_[static_cast<size_t>(PipelineType::Object3D)]         = std::make_unique<Object3DPipeline>();
    pipelines_[static_cast<size_t>(PipelineType::SkinningObject3D)] = std::make_unique<SkinningObject3DPipeline>();
    pipelines_[static_cast<size_t>(PipelineType::Particle)]         = std::make_unique<ParticlePipeline>();
    pipelines_[static_cast<size_t>(PipelineType::GPUParticle)]      = std::make_unique<GPUParticlePipeline>();
    pipelines_[static_cast<size_t>(PipelineType::ShadowMap)]        = std::make_unique<ShadowMapPipeline>();
    pipelines_[static_cast<size_t>(PipelineType::Line3D)]           = std::make_unique<Line3DPipeline>();

    // 全パイプラインを作成・初期化
    for (size_t i = 0; i < static_cast<size_t>(PipelineType::Count); ++i) {
        pipelines_[i]->Init(dxCommon);
    }
}

BasePipeline* PipelineManager::GetPipeline(const PipelineType& type) const {

    size_t index = static_cast<size_t>(type);
    return pipelines_[index].get();
}

void PipelineManager::PreDraw(const PipelineType& type, ID3D12GraphicsCommandList* commandList) const {
    size_t index = static_cast<size_t>(type);
    assert(pipelines_[index]);
    pipelines_[index]->PreDraw(commandList);
}

void PipelineManager::PreBlendSet(const PipelineType& type, ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) const {
    size_t index = static_cast<size_t>(type);
    assert(pipelines_[index]);
    pipelines_[index]->PreBlendSet(commandList, blendMode);
}