#pragma once
#include "../../ParticleManager.h"

namespace KetaEngine {

/// <summary>
/// パラメータからパーティクル1個分のインスタンスを生成するファクトリクラス
/// </summary>
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
