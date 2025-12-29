#pragma once

#include "../math/MinMax.h"
#include "Vector3.h"
#include <cstdint>

namespace KetaEngine {

struct ParticleEmit {
    Vector3 translate;
    uint32_t count;
    float frequency;
    float frequencyTime;
    uint32_t emit;
    float _padding0;
};


struct EmitTransformParams {
    Vector3 scaleMin;
    float _padding0;
    Vector3 scaleMax;
    float _padding1;

    Vector3 rotationMin;
    float _padding2;
    Vector3 rotationMax;
    float _padding3;

    Vector3 translateMin;
    float _padding4;
    Vector3 translateMax;
    float _padding5;
};

// Physics系パラメータ
struct EmitPhysicsParams {
    Vector3 velocityMin;
    float _padding0;
    Vector3 velocityMax;
    float _padding1;

    Vector3 rotationSpeedMin; 
    float _padding2;
    Vector3 rotationSpeedMax;
    float _padding3;
};

// Appearance系パラメータ
struct EmitAppearanceParams {
    Vector4 colorMin;
    Vector4 colorMax;

    float lifeTimeMin;
    float lifeTimeMax;

    float _padding0;
    float _padding1;
    float _padding2;
};

struct PerFrame {
    float time;
    float deltaTime;
};

}; // KetaEngine
