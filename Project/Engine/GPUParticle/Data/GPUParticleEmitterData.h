#pragma once

#include "../math/MinMax.h"
#include "Vector3.h"
#include <cstdint>

struct ParticleEmit {
    Vector3 translate;
    float radius;
    uint32_t count;
    float frequency;
    float frequencyTime;
    uint32_t emit;
};


struct EmitParameter {
    Vector3 scaleMin;
    float _padding0; 
    Vector3 scaleMax;
    float _padding1;

    Vector3 rotationMin;
    float _padding2;
    Vector3 rotationMax;
    float _padding3;

    Vector3 rotationSpeedMin;
    float _padding4;
    Vector3 rotationSpeedMax;
    float _padding5;

    Vector3 translateMin;
    float _padding6;
    Vector3 translateMax;
    float _padding7;

    Vector3 velocityMin;
    float _padding8;
    Vector3 velocityMax;
    float _padding9;

    Vector4 colorMin;
    Vector4 colorMax;

    float lifeTimeMin;
    float lifeTimeMax;
    float _padding10;
    float _padding11; 
};

struct PerFrame {
    float time;
    float deltaTime;
};