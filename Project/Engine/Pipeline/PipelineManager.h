#pragma once

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>
#include <d3d12.h>
#include <Material/ModelMaterial.h>

class DirectXCommon;
class BasePipeline;
class SpritePipeline;
class Object3DPipeline;
class SkinningObject3DPipeline;
class ParticlePipeline;
class GPUParticlePipeline;
class ShadowMapPipeline;
class Line3DPipeline;

enum class PipelineType {
    Sprite,
    Object3D,
    SkinningObject3D,
    Particle,
    GPUParticle,
    ShadowMap,
    Line3D,
    Count 
};

class PipelineManager {
public:
    static PipelineManager* GetInstance();

    PipelineManager(const PipelineManager&)            = delete;
    PipelineManager& operator=(const PipelineManager&) = delete;
    PipelineManager(PipelineManager&&)                 = delete;
    PipelineManager& operator=(PipelineManager&&)      = delete;

    void Init(DirectXCommon* dxCommon);
    void PreDraw(const PipelineType& type, ID3D12GraphicsCommandList* commandList) const;
    void PreBlendSet(const PipelineType& type, ID3D12GraphicsCommandList* commandList, const BlendMode& blendMode) const;
 
private:
    PipelineManager()  = default;
    ~PipelineManager() = default;

  
private:
    std::array<std::unique_ptr<BasePipeline>, static_cast<size_t>(PipelineType::Count)> pipelines_;
    DirectXCommon* dxCommon_ = nullptr;

public:
    // getter
    BasePipeline* GetPipeline(const PipelineType& type) const;
};