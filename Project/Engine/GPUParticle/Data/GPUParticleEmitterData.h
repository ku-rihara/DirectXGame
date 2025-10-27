#pragma once

#include"Vector3.h"
#include"../math/MinMax.h"
#include<cstdint>

struct ParticleEmit {
    Vector3 translate;
    float radius;
    uint32_t count;
    float frequency;
    float frequencyTime;
    uint32_t emit;
};

struct EmitParameter {
    V3MinMax scaleRange;
    V3MinMax rotationRange;
    V3MinMax rotationSpeedRange;
    V3MinMax translateRange;
    V3MinMax velocityRange;
    V4MinMax colorRange;
    FMinMax lifeTimeRange;
   
};

struct PerFrame {
    float time;
    float deltaTime;
};