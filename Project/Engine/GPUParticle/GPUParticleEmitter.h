#pragma once
#include "Data/GPUParticleEmitterData.h"
#include "Vector3.h"
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

public:
    void SetEmitterData(const EmitterSphere& data);
    const EmitterSphere& GetEmitterData() const;
    float GetFrequencyTime() const;
    uint32_t GetEmitFlag() const;
};