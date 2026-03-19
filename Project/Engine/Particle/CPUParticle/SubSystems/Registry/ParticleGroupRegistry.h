#pragma once
#include "../../ParticleManager.h"
#include "3D/Primitive/IPrimitive.h"
#include <string>
#include <unordered_map>

namespace KetaEngine {

class ParticleGroupRegistry {
public:
    ParticleGroupRegistry()  = default;
    ~ParticleGroupRegistry() = default;

    void CreateParticleGroup(
        const std::string& name,
        const std::string& modelFilePath,
        uint32_t maxnum,
        std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
        SrvManager* srvManager);

    void CreatePrimitiveParticle(
        const std::string& name,
        PrimitiveType type,
        uint32_t maxnum,
        std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups,
        SrvManager* srvManager);

    void ResetInstancingData(
        const std::string& name,
        ParticleManager::ParticleGroup& group);

    void ResetAllParticles(
        std::unordered_map<std::string, ParticleManager::ParticleGroup>& groups);

private:
    void CreateInstancingResource(
        const std::string& name,
        uint32_t instanceNum,
        ParticleManager::ParticleGroup& group,
        SrvManager* srvManager);

    void CreateMaterialResource(ParticleManager::ParticleGroup& group);

    void SetModel(
        const std::string& modelName,
        ParticleManager::ParticleGroup& group);
};

} // namespace KetaEngine
