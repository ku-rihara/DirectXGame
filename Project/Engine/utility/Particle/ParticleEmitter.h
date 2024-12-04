#pragma once

#include "MinMax.h"
#include <string>
#include"GrobalParamater/GlobalParameter.h"

/// <summary>
/// パーティクルエミッタ
/// </summary>
class ParticleEmitter {
private:
  

    std::string particleName_;
    Vector3 basePos_;
    V3MinMax positionDist_;
    V3MinMax scaleDist_;
    V3MinMax velocityDist_;
    V4MinMax colorDist_;
    Vector4 baseColor_;
    float lifeTime_;
    int32_t particleCount_;

    const std::string dyrectryPath = "./Resources/ParticleParamater/";
   

public:
    ParticleEmitter();
    static ParticleEmitter* CreateParticle(const std::string& name, const std::string& modelFilePath, const std::string& extension, const int32_t& maxnum);

    void Init();
    void Emit();
    void EditorUpdate();
};
