#pragma once
#include "../../ParticleManager.h"

namespace KetaEngine {

class ParticleFactory {
public:
    ParticleFactory()  = default;
    ~ParticleFactory() = default;

    ParticleManager::Particle MakeParticle(
        const ParticleManager::Parameters& parameters,
        const ParticleManager::DissolveGroupParams* dissolveParams,
        const ViewProjection* viewProjection);
};

} // namespace KetaEngine
