#pragma once
#include "../../ParticleManager.h"
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace KetaEngine {

class ParticleRenderer {
public:
    ParticleRenderer()  = default;
    ~ParticleRenderer() = default;

    void Draw(std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
              const ViewProjection& viewProjection,
              SrvManager* srvManager);

    void DrawScreenPos(std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
                       SrvManager* srvManager);

private:
    void AlphaAdapt(ParticleFprGPU& data,
                    const ParticleManager::Particle& parm,
                    const ParticleManager::ParticleGroup& group);

    uint32_t BuildInstancingData(
        ParticleManager::ParticleGroup& group,
        ParticleFprGPU* instancingData,
        const Matrix4x4& matView,
        const Matrix4x4& matProj);

    void DrawGroup(ParticleManager::ParticleGroup& group,
                   uint32_t instanceIndex,
                   SrvManager* srvManager,
                   ID3D12GraphicsCommandList* commandList);
};

} // namespace KetaEngine
