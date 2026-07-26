#pragma once
#include "../../ParticleManager.h"

namespace KetaEngine {

/// <summary>
/// パーティクルグループの物理・UV更新を行うクラス
/// </summary>
class ParticleUpdater {
public:
    ParticleUpdater()  = default;
    ~ParticleUpdater() = default;

    void UpdateGroup(ParticleManager::ParticleGroup& group,
                     const ParticleManager::AccelerationField& field,
                     const ViewProjection* viewProjection);

    void UpdateUV(ParticleManager::UVInfo& uvInfo, float deltaTime);

private:
    void UpdateDissolvePlayer(ParticleManager::ParticleGroup& group);
};

} // namespace KetaEngine
