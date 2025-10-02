#pragma once
#include "Vector3.h"
#include"Data/GPUParticleEmitterData.h"
#include <cstdint>

class GPUParticleEmitter {
public:
 
public:
    GPUParticleEmitter()  = default;
    ~GPUParticleEmitter() = default;

    void Init();
    void Update();

private:
    EmitterSphere emitterSphere_;
};