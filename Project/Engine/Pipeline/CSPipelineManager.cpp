#include "CSPipelineManager.h"

using namespace KetaEngine;

// Pipeline includes
#include "Pipeline/Skinning/SkinningCSPipeline.h"
#include"Pipeline/Particle/CS/EmitParticlePipeline.h"
#include"Pipeline/Particle/CS/InitParticlePipeline.h"
#include"Pipeline/Particle/CS/UpdateParticlePipeline.h"

#include "Dx/DirectXCommon.h"
#include <cassert>

CSPipelineManager* CSPipelineManager::GetInstance() {
    static CSPipelineManager instance;
    return &instance;
}

void CSPipelineManager::Init(DirectXCommon* dxCommon) {
    assert(dxCommon);

    dxCommon_ = dxCommon;

    // 生成
    pipelines_[static_cast<size_t>(CSPipelineType::Skinning)] = std::make_unique<SkinningCSPipeline>();
    pipelines_[static_cast<size_t>(CSPipelineType::Particle_Init)] = std::make_unique<InitParticlePipeline>();
    pipelines_[static_cast<size_t>(CSPipelineType::Particle_Emit)] = std::make_unique<EmitParticlePipeline>();
    pipelines_[static_cast<size_t>(CSPipelineType::Particle_Update)] = std::make_unique<UpdateParticlePipeline>();
 

    // 全パイプラインを作成・初期化
    for (size_t i = 0; i < static_cast<size_t>(CSPipelineType::Count); ++i) {
        pipelines_[i]->Init(dxCommon);
    }
}

BaseCSPipeline* CSPipelineManager::GetPipeline(const CSPipelineType& type) const {
    size_t index = static_cast<size_t>(type);
    return pipelines_[index].get();
}

void CSPipelineManager::PreDraw(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList) const {
    size_t index = static_cast<size_t>(type);
    assert(pipelines_[index]); 
    pipelines_[index]->PreDraw(commandList);
}

void CSPipelineManager::DisPatch(const CSPipelineType& type, ID3D12GraphicsCommandList* commandList, const UINT& numThreadsX) {
    size_t index = static_cast<size_t>(type);
    assert(pipelines_[index]);
    pipelines_[index]->Dispatch(commandList,numThreadsX);
 }
